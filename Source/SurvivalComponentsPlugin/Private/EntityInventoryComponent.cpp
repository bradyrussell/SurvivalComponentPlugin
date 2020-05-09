// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityInventoryComponent.h"
#include "DatabaseProvider.h"
#include "ConsumableItemEffectBase.h"
#include "Net/UnrealNetwork.h"


UEntityInventoryComponent::UEntityInventoryComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UEntityInventoryComponent::BeginPlay() {
	Super::BeginPlay();
	if(GetOwnerRole() == ROLE_Authority){
	EquipmentSlots.AddDefaulted(NumberEquipmentSlots);
	CurrentEquipmentEffects.AddDefaulted(NumberEquipmentSlots);
	}
}

void UEntityInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	for(auto &elem:CurrentEquipmentEffects) {
		if(elem != nullptr && elem->IsValidLowLevel()) elem->OnEquipmentTick(DeltaTime, Cast<APawn>(this->GetOwner()));
	}
}

bool UEntityInventoryComponent::ConsumeItem(int32 Slot, APawn* Cause, AController* Instigator, float MagnitudeMultiplier) {
	const FItemStack Item = InventorySlots[Slot];
	if (UBaseInventoryComponent::isEmpty(Item))
		return false;

	const auto ItemDef = UDatabaseProvider::GetItemDefinition(this, Item.Item);

	if (ItemDef.bIsConsumable) {
		auto EffectObject = NewObject<UConsumableItemEffectBase>(this, ItemDef.ItemEffect);
		const auto FinalMagnitude = ItemDef.ItemEffectMagnitude * MagnitudeMultiplier;

		const auto bConsumeItem = EffectObject->OnItemConsumed(Cause, Instigator, FinalMagnitude);

		UE_LOG(LogTemp, Warning, TEXT("EXECUTING -> Consumable Item Effect (%s) with magnitude %f: Consumed %i"), *EffectObject->GetName(), FinalMagnitude, bConsumeItem);

		EffectObject->ConditionalBeginDestroy(); // we immediately destroy the effect object
		EffectObject = nullptr;

		if (bConsumeItem) { InventorySlots[Slot].Amount--; }

		return bConsumeItem;
	}

	return false;
}

bool UEntityInventoryComponent::EquipItem(int32 Slot, uint8 EquipmentSlot, APawn* Cause, AController* Instigator) {
	if(!EquipmentSlots[EquipmentSlot].isEmpty()) return false;
	
	const FItemStack Item = InventorySlots[Slot];
	if (UBaseInventoryComponent::isEmpty(Item)) return false;

	const auto ItemDef = UDatabaseProvider::GetItemDefinition(this, Item.Item);

	if (ItemDef.ValidEquipmentSlots.Contains(EquipmentSlot)) {
		InventorySlots[Slot] = FItemStack(); // swap the slots
		EquipmentSlots[EquipmentSlot] = Item;

		CurrentEquipmentEffects[EquipmentSlot] = NewObject<UEquipmentItemEffectBase>(this, ItemDef.ItemEffect);
		
		CurrentEquipmentEffects[EquipmentSlot]->OnItemEquipped(Cause, Instigator);
		
		UE_LOG(LogTemp, Warning, TEXT("EXECUTING -> Equip Item Effect (%s)"), *CurrentEquipmentEffects[EquipmentSlot]->GetName());
		return true;
	}
	return false;
}

bool UEntityInventoryComponent::UnequipItem(int32 Slot, uint8 EquipmentSlot, APawn* Cause, AController* Instigator) {
	if(EquipmentSlots[EquipmentSlot].isEmpty() || !UBaseInventoryComponent::isEmpty(InventorySlots[Slot])) return false;

	const auto Item = EquipmentSlots[EquipmentSlot];
	EquipmentSlots[EquipmentSlot] = FItemStack();
	InventorySlots[Slot] = Item;

	CurrentEquipmentEffects[EquipmentSlot]->OnItemUnequipped(Cause, Instigator);
	UE_LOG(LogTemp, Warning, TEXT("EXECUTING -> Unequip Item Effect (%s)"), *CurrentEquipmentEffects[EquipmentSlot]->GetName());
	
	CurrentEquipmentEffects[EquipmentSlot]->ConditionalBeginDestroy();
	CurrentEquipmentEffects[EquipmentSlot] = nullptr;
	return true;
}

bool UEntityInventoryComponent::OnEquipmentHit(float Damage,
	AActor* DamageCause,
	AController* DamageInstigator,
	TSubclassOf<UDamageType> DamageType,
	APawn* Wearer,
	AController* WearerController) {
	
	for(auto &elem:CurrentEquipmentEffects) {
		if(elem != nullptr && elem->IsValidLowLevel()) {
			elem->OnEquipmentHit(Damage,DamageCause, DamageInstigator, DamageType, Wearer, WearerController);
			UE_LOG(LogTemp, Warning, TEXT("EXECUTING -> OnEquipmentHit Item Effect (%s)"), *elem->GetName());
		}
	}
	return true;
}

void UEntityInventoryComponent::Server_UnequipItem_Implementation(int32 Slot, uint8 EquipmentSlot, APawn* Cause, AController* Instigator) {
	UnequipItem(Slot,EquipmentSlot,Cause,Instigator);
}

bool UEntityInventoryComponent::Server_UnequipItem_Validate(int32 Slot, uint8 EquipmentSlot, APawn* Cause, AController* Instigator) {
		return true; //todo
}

void UEntityInventoryComponent::Server_EquipItem_Implementation(int32 Slot, uint8 EquipmentSlot, APawn* Cause, AController* Instigator) {
	EquipItem(Slot,EquipmentSlot,Cause,Instigator);
}

bool UEntityInventoryComponent::Server_EquipItem_Validate(int32 Slot, uint8 EquipmentSlot, APawn* Cause, AController* Instigator) {
		return true; //todo
}

void UEntityInventoryComponent::Server_ConsumeItem_Implementation(int32 Slot, APawn* Cause, AController* Instigator, float MagnitudeMultiplier) {
	ConsumeItem(Slot,Cause, Instigator, MagnitudeMultiplier);
}

bool UEntityInventoryComponent::Server_ConsumeItem_Validate(int32 Slot, APawn* Cause, AController* Instigator, float MagnitudeMultiplier) {
	return true; //todo
}

void UEntityInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(UEntityInventoryComponent, NumberEquipmentSlots);
	DOREPLIFETIME(UEntityInventoryComponent, EquipmentSlots);
}
