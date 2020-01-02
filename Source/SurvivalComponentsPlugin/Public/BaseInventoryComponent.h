// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryStructs.h"
#include "Components/ActorComponent.h"
#include "BaseInventoryComponent.generated.h"

#define DBG_MAX_STACK 256

UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
	class SURVIVALCOMPONENTSPLUGIN_API UBaseInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseInventoryComponent();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated, Category="Inventory") int32 NumberSlots;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Inventory") TArray<FItemStack> InventorySlots;

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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") bool TransferAllToInventory(UBaseInventoryComponent* Recipient);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") FItemStack ExchangeItem(int32 Slot, FItemStack NewItem);

	// returns whether the inventory contains at least the given Amount of Item
	UFUNCTION(BlueprintCallable, Category="Inventory") bool hasItem(FItemStack Item);

	//returns whether the inventory contains at least the given Amounts of Items
	UFUNCTION(BlueprintCallable, Category="Inventory") bool hasItems(TArray<FItemStack> Items);

	//returns a copy of the current state of the inventory
	UFUNCTION(BlueprintCallable, Category="Inventory") TArray<FItemStack> GetInventorySnapshot() const;

	// returns a reference to the inventory array. do not modify it
	UFUNCTION(BlueprintCallable, Category="Inventory") void GetInventoryReference(TArray<FItemStack>& InventoryRef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void ResizeInventory(int32 NewNumberSlots);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void ClearInventory();

	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetFirstEmptySlot();
	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetNumberEmptySlots();
	UFUNCTION(BlueprintPure, Category="Inventory") int32 GetNumberFilledSlots();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory") void SwapSlots(int32 FirstSlot, int32 SecondSlot);

	// returns true if all items are empty
	UFUNCTION(BlueprintPure, Category="Inventory") static bool areAllEmpty(TArray<FItemStack> Items);

	UFUNCTION(BlueprintPure, Category="Inventory") static bool isEmpty(FItemStack Item);

	UFUNCTION(BlueprintPure, Category="Inventory") static FString ToString(FItemStack Item);
	UFUNCTION(BlueprintPure, Category="Inventory") static FString ToStrings(TArray<FItemStack> Items);

	UFUNCTION(BlueprintCallable, Category="Inventory") void SortInventory();

	//// data lookups below

	//UFUNCTION(BlueprintPure, Category="Inventory") int32 GetMaxStackForItem(FName item) const;
};
