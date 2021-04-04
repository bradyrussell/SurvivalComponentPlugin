// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInventoryComponent.h"
#include "Engine/Engine.h"
#include "DatabaseProvider.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBaseInventoryComponent::UBaseInventoryComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UBaseInventoryComponent::BeginPlay() {
	Super::BeginPlay();

	InventorySlots.AddDefaulted(NumberSlots);
	// ...

}

bool UBaseInventoryComponent::AreItemsEqual(const FItemStack& A, const FItemStack& B, bool bMatchData) {
	return A.Item == B.Item &&  (!bMatchData || A.Data == B.Data);
}

bool UBaseInventoryComponent::DoItemsStack(const FItemStack& A, const FItemStack& B) {
	return AreItemsEqual(A,B,true);
}

FItemStack UBaseInventoryComponent::AddItem(FItemStack NewItem) {
	const auto MaxStack = UDatabaseProvider::GetItemDefinition(this, NewItem.Item).MaxStack;
	//do we already have any of this item?
	for (auto& elem : InventorySlots) {
		if (!elem.IsEmpty() && DoItemsStack(elem, NewItem)) {
			// how much can we put in this stack
			if (elem.Amount < MaxStack) {
				// how much more can we put in this stack
				const int AmountTaken = FMath::Min(NewItem.Amount, MaxStack - elem.Amount); // either all of the new items OR as much as we can 

				NewItem.Amount -= AmountTaken;
				elem.Amount += AmountTaken; // perform swap
			}
		}
	}

	// do we have leftover items?
	while (NewItem.Amount > 0) {
		// can we make a new stack?
		const int emptySlot = GetFirstEmptySlot();

		if (InventorySlots.IsValidIndex(emptySlot)) {
			const int newStackAmount = FMath::Min(NewItem.Amount, MaxStack);
			NewItem.Amount -= newStackAmount;
			InventorySlots[emptySlot] = FItemStack(NewItem.Item, newStackAmount, NewItem.Data);
		}
		else {
			return NewItem;
		}
	}
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
	return FItemStack();
}

TArray<FItemStack> UBaseInventoryComponent::AddItems(TArray<FItemStack> NewItems) {
	if (NewItems.Num() == 0)
		return NewItems;

	TArray<FItemStack> excess;

	for (auto& elem : NewItems) {
		auto excessStack = AddItem(elem);
		if (!excessStack.IsEmpty())
			excess.Add(excessStack);
	}
	return excess;
}

FItemStack UBaseInventoryComponent::RemoveItem(FItemStack Item, bool bMatchData) {
	for (auto& elem : InventorySlots) {
		if (AreItemsEqual(elem, Item, bMatchData)) {
			const int32 AmountRemoved = FMath::Min(elem.Amount, Item.Amount); // the lesser amount
			elem.Amount -= AmountRemoved;
			Item.Amount -= AmountRemoved;

			if (Item.Amount <= 0) {
				if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
				return Item;
			}	
		}
	}
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
	return Item;
}

TArray<FItemStack> UBaseInventoryComponent::RemoveItems(TArray<FItemStack> Items, bool bMatchData) {
	TArray<FItemStack> excess;

	for (auto& elem : Items) {
		auto excessStack = RemoveItem(elem, bMatchData);
		if (!excessStack.IsEmpty())
			excess.Add(excessStack);
	}
	return excess;
}

bool UBaseInventoryComponent::TransferToInventory(UBaseInventoryComponent* Recipient, int32 Slot) {
	if(!bCanGiveItems || !Recipient->bCanReceiveItems) return false;
	const auto ItemInSlot = InventorySlots[Slot];
	if (ItemInSlot.IsEmpty())
		return false;

	InventorySlots[Slot] = Recipient->AddItem(ItemInSlot); // move into recipient, leaving overflow
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
	return true;
}

bool UBaseInventoryComponent::TransferToInventorySlot(UBaseInventoryComponent* Recipient, int32 FromSlot, int32 ToSlot) {
	if(!bCanGiveItems || !Recipient->bCanReceiveItems) return false;
	const auto ItemInSlot = InventorySlots[FromSlot];
	if (ItemInSlot.IsEmpty())
		return false;

	if(!Recipient->InventorySlots.IsValidIndex(ToSlot) || !InventorySlots.IsValidIndex(FromSlot)) return false;
	
	if (!DoItemsStack(Recipient->InventorySlots[ToSlot], ItemInSlot) || Recipient->InventorySlots[ToSlot].IsEmpty()) {
		InventorySlots[FromSlot] = Recipient->InventorySlots[ToSlot];
		Recipient->InventorySlots[ToSlot] = ItemInSlot;
		if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
		return true;
	}
	else {
		const auto MaxStack = UDatabaseProvider::GetItemDefinition(this, ItemInSlot.Item).MaxStack;
		const int AbleToTake = MaxStack - Recipient->InventorySlots[ToSlot].Amount;
		const int AmountTaken = FMath::Min(AbleToTake, ItemInSlot.Amount);
		InventorySlots[FromSlot].Amount -= AmountTaken;
		Recipient->InventorySlots[ToSlot].Amount += AmountTaken;
		if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
		return true;
	}

}

bool UBaseInventoryComponent::TransferAllToInventory(UBaseInventoryComponent* Recipient) {
	if(!bCanGiveItems || !Recipient->bCanReceiveItems) return false;
	if (GetNumberFilledSlots() == 0)
		return false;
	const auto overflow = Recipient->AddItems(InventorySlots);
	ClearInventory();
	AddItems(overflow);
	return true;
}

FItemStack UBaseInventoryComponent::ExchangeItem(int32 Slot, FItemStack NewItem) {
	const auto Old = InventorySlots[Slot];
	InventorySlots[Slot] = NewItem;
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
	return Old;
}

void UBaseInventoryComponent::OnRep_InventorySlots() {
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_InventorySlots()"));
}


bool UBaseInventoryComponent::HasItem(FItemStack Item, bool bMatchData) {
	for (auto& elem : InventorySlots) {
		if (AreItemsEqual(Item, elem, bMatchData)) { Item.Amount -= elem.Amount; }

		if (Item.Amount <= 0)
			return true; // stop early
	}
	return false;
}

bool UBaseInventoryComponent::HasItems(TArray<FItemStack> Items, bool bMatchData) {
	for (auto& elem : Items) {
		if (!HasItem(elem, bMatchData))
			return false;
	}
	return true;
}

TArray<FItemStack> UBaseInventoryComponent::GetInventorySnapshot() const { return InventorySlots; }

void UBaseInventoryComponent::GetInventoryReference(TArray<FItemStack>& InventoryRef) const { InventoryRef = InventorySlots; }


void UBaseInventoryComponent::ResizeInventory(int32 NewNumberSlots) {
	const auto SlotsCopy = InventorySlots;
	InventorySlots.Reset(NewNumberSlots);
	InventorySlots.AddDefaulted(NewNumberSlots);
	NumberSlots = NewNumberSlots;
	AddItems(SlotsCopy);
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
}

void UBaseInventoryComponent::ClearInventory() {
	InventorySlots.Reset();
	InventorySlots.AddDefaulted(NumberSlots);
	if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
}

int32 UBaseInventoryComponent::GetFirstEmptySlot() {
	for (int i = 0; i < InventorySlots.Num(); i++) {
		if (InventorySlots[i].IsEmpty())
			return i;
	}
	return -1;
}

int32 UBaseInventoryComponent::GetFirstFilledSlot() {
	for (int i = 0; i < InventorySlots.Num(); i++) {
		if (!InventorySlots[i].IsEmpty())
			return i;
	}
	return -1;
}

int32 UBaseInventoryComponent::GetNumberEmptySlots() {
	int32 empty = 0;
	for (auto& elem : InventorySlots) {
		if (elem.IsEmpty())
			empty++;
	}
	return empty;
}

int32 UBaseInventoryComponent::GetNumberFilledSlots() { return NumberSlots - GetNumberEmptySlots(); }

void UBaseInventoryComponent::SwapSlots(int32 FirstSlot, int32 SecondSlot) { InventorySlots.Swap(FirstSlot, SecondSlot); }

void UBaseInventoryComponent::SwapOrCombineSlots(int32 FirstSlot, int32 SecondSlot) {
	if (DoItemsStack(InventorySlots[FirstSlot], InventorySlots[SecondSlot]) && !InventorySlots[FirstSlot].IsEmpty() && !InventorySlots[SecondSlot].IsEmpty()) {
		const auto MaxStack = UDatabaseProvider::GetItemDefinition(this, InventorySlots[SecondSlot].Item).MaxStack;

		const int AbleToTake = MaxStack - InventorySlots[SecondSlot].Amount;

		if (AbleToTake == 0) { SwapSlots(FirstSlot, SecondSlot); }
		else {
			const int AmountTaken = FMath::Min(AbleToTake, InventorySlots[FirstSlot].Amount);
			InventorySlots[FirstSlot].Amount -= AmountTaken;
			InventorySlots[SecondSlot].Amount += AmountTaken;
			if(InventoryChanged_Event.IsBound()) InventoryChanged_Event.Broadcast(true);
		}
	}
	else { SwapSlots(FirstSlot, SecondSlot); }
}

bool UBaseInventoryComponent::IsEmpty(FItemStack Item) {
	return Item.IsEmpty();
}

bool UBaseInventoryComponent::AreAllEmpty(TArray<FItemStack> Items) {
	for (auto& elem : Items) {
		if (!elem.IsEmpty()) return false;
	}
	return true;
}

FString UBaseInventoryComponent::ToString(FItemStack Item) {
	if (Item.IsEmpty())
		return FString(TEXT("Empty"));
	if(Item.HasData()) return *FString::Printf(TEXT("%s: %i [%s]"), *Item.Item.ToString(), Item.Amount, *Item.Data);
	return *FString::Printf(TEXT("%s: %i"), *Item.Item.ToString(), Item.Amount);
}

FString UBaseInventoryComponent::ToStrings(TArray<FItemStack> Items) {
	FString out("");
	for (auto& elem : Items) { out.Append(ToString(elem) + LINE_TERMINATOR); }
	return out;
}

void UBaseInventoryComponent::SortInventory() { InventorySlots.Sort(); }

void UBaseInventoryComponent::Server_TransferToInventorySlot_Implementation(UBaseInventoryComponent* Recipient, int32 FromSlot, int32 ToSlot) {
	TransferToInventorySlot(Recipient, FromSlot, ToSlot);
}

bool UBaseInventoryComponent::Server_TransferToInventorySlot_Validate(UBaseInventoryComponent* Recipient, int32 FromSlot, int32 ToSlot) {
	//UE_LOG(LogTemp, Warning, TEXT("UBaseInventoryComponent::Server_TransferToInventorySlot_Validate failed due to null recipient."));
	return Recipient && FromSlot < NumberSlots && ToSlot < Recipient->NumberSlots && FMath::Min(FromSlot, ToSlot) >= 0;
}

void UBaseInventoryComponent::Server_SwapOrCombineSlots_Implementation(int32 FirstSlot, int32 SecondSlot) {
	SwapOrCombineSlots(FirstSlot, SecondSlot);
}

bool UBaseInventoryComponent::Server_SwapOrCombineSlots_Validate(int32 FirstSlot, int32 SecondSlot) {
	return FMath::Max(FirstSlot, SecondSlot) < NumberSlots && FMath::Min(FirstSlot, SecondSlot) >= 0;
}


void UBaseInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBaseInventoryComponent, NumberSlots);
	DOREPLIFETIME(UBaseInventoryComponent, InventorySlots);
	DOREPLIFETIME(UBaseInventoryComponent, bCanReceiveItems);
	DOREPLIFETIME(UBaseInventoryComponent, bCanGiveItems);
	//DOREPLIFETIME_CONDITION_NOTIFY(UBaseInventoryComponent, InventorySlots, COND_None, REPNOTIFY_Always);
}
