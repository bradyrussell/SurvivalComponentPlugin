// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryStructs.h"
#include "Components/ActorComponent.h"
#include "BaseInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryChanged, bool, bShouldRefresh);

UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), Blueprintable )
	class SURVIVALCOMPONENTSPLUGIN_API UBaseInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category="Inventory - Events") FInventoryChanged InventoryChanged_Event;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated, Category="Inventory", SaveGame) int32 NumberSlots;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, ReplicatedUsing=OnRep_InventorySlots, Category="Inventory", SaveGame) TArray<FItemStack> InventorySlots;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:

	//adds an item stack to the inventory, returning any excess items
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") FItemStack AddItem(FItemStack NewItem);

	//adds each item stack to the inventory, returning any excess items
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") TArray<FItemStack> AddItems(TArray<FItemStack> NewItems);

	// removes the amount of the item from the inv, returning the items it could not remove
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") FItemStack RemoveItem(FItemStack Item);

	// removes the amounts of the items from the inv, returning the items it could not remove
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") TArray<FItemStack> RemoveItems(TArray<FItemStack> Items);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") bool TransferToInventory(UBaseInventoryComponent* Recipient, int32 Slot);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") bool TransferToInventorySlot(UBaseInventoryComponent* Recipient, int32 FromSlot, int32 ToSlot);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") bool TransferAllToInventory(UBaseInventoryComponent* Recipient);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") bool TransferAmountToInventory(UBaseInventoryComponent* Recipient, int32 NumberOfItems);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") FItemStack ExchangeItem(int32 Slot, FItemStack NewItem);

	UFUNCTION() virtual void OnRep_InventorySlots();
	
	// returns whether the inventory contains at least the given Amount of Item
	UFUNCTION(BlueprintCallable, Category="Inventory") bool hasItem(FItemStack Item);

	//returns whether the inventory contains at least the given Amounts of Items
	UFUNCTION(BlueprintCallable, Category="Inventory") bool hasItems(TArray<FItemStack> Items);

	//returns a copy of the current state of the inventory
	UFUNCTION(BlueprintCallable, Category="Inventory") TArray<FItemStack> GetInventorySnapshot() const;

	// returns a reference to the inventory array. do not modify it
	UFUNCTION(BlueprintCallable, Category="Inventory") void GetInventoryReference(TArray<FItemStack>& OutInventoryRef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void ResizeInventory(int32 NewNumberSlots);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void ClearInventory();

	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetFirstFilledSlot();
	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetFirstEmptySlot();
	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetNumberEmptySlots();
	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetNumberFilledSlots();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void SwapSlots(int32 FirstSlot, int32 SecondSlot);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void SwapOrCombineSlots(int32 FirstSlot, int32 SecondSlot);

	// returns true if all items are empty
	UFUNCTION(BlueprintPure, Category="Inventory") static bool areAllEmpty(TArray<FItemStack> Items);

	UFUNCTION(BlueprintPure, Category="Inventory") static bool isEmpty(FItemStack Item);

	UFUNCTION(BlueprintPure, Category="Inventory") static FString ToString(FItemStack Item);
	UFUNCTION(BlueprintPure, Category="Inventory") static FString ToStrings(TArray<FItemStack> Items);

	UFUNCTION(BlueprintCallable, Category="Inventory") void SortInventory();

	/* Client to Server functions - useful for UI */
	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Inventory") void Server_SortInventory();
	
	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Inventory") void Server_TransferToInventory(UBaseInventoryComponent* Recipient, int32 Slot);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Inventory") void Server_TransferToInventorySlot(UBaseInventoryComponent* Recipient, int32 FromSlot, int32 ToSlot);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Inventory") void Server_TransferAllToInventory(UBaseInventoryComponent* Recipient);

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category="Inventory") void Server_SwapOrCombineSlots(int32 FirstSlot, int32 SecondSlot);

	/* End of client to server functions */
	
	//// data lookups below

	//UFUNCTION(BlueprintPure, Category="Inventory") int32 GetMaxStackForItem(FName item) const;
};
