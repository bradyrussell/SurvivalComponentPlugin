// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ContainerInventoryComponent.h"
#include "Engine/DataTable.h"
#include "CraftingInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBeginProcessingRecipe, FProcessingRecipe, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFinishProcessingRecipe, FProcessingRecipe, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFailedProcessingRecipe, FProcessingRecipe, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQueuedProcessingRecipe, FProcessingRecipe, Recipe);

/**
 * 
 */
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
	class SURVIVALCOMPONENTSPLUGIN_API UCraftingInventoryComponent : public UContainerInventoryComponent {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated, Category="Inventory - Crafting") ECraftingType Type;

	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FBeginProcessingRecipe BeginProcessingRecipe_Event;
	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FFinishProcessingRecipe FinishProcessingRecipe_Event;
	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FFailedProcessingRecipe FailedProcessingRecipe_Event;
	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FQueuedProcessingRecipe QueuedProcessingRecipe_Event;

	TQueue<FProcessingRecipe> ProcessingQueue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Inventory - Crafting") float Fuel = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Inventory - Crafting") float MaximumFuel = 10000.f;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Replicated, Category="Inventory - Crafting") bool bIsCurrentlyProcessing = false;

	UPROPERTY(BlueprintReadWrite, Category="Inventory - Crafting") FTimerHandle CurrentJob_Timer;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Replicated, Category="Inventory - Crafting") FProcessingRecipe CurrentJob_Recipe;

	//  replicate the expected finish time of the recipe for client side visualization
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) FDateTime Client_ExpectedFinish;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") bool QueueRecipe(FProcessingRecipe InRecipe);

	// will begin working through the queued recipes
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") bool BeginProcessing();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") FProcessingRecipe PeekNextRecipe() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") bool AddFuel(float Amount);


	
private:
	//called when a recipe finishes. 
	void EndProcessing();

	UFUNCTION(NetMulticast, Reliable) void Multi_BeginProcessingRecipe(FProcessingRecipe Recipe);
	UFUNCTION(NetMulticast, Reliable) void Multi_FinishProcessingRecipe(FProcessingRecipe Recipe);
	UFUNCTION(NetMulticast, Reliable) void Multi_FailedProcessingRecipe(FProcessingRecipe Recipe);
	UFUNCTION(NetMulticast, Reliable) void Multi_QueuedProcessingRecipe(FProcessingRecipe Recipe);

	// // keeps the client timer in sync with the server timer
	// UFUNCTION(Client, Reliable) void Client_SetTimer(float duration);
	// void Client_TimerFinished(); // todo find out how to do a null timer
public:
	//interrupt crafting, does not clear queue
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") void CancelProcessing();

	// clear queue. does not cancel in progress recipe
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") void ClearQueue();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") void CancelAndClearQueue();

	UFUNCTION(BlueprintCallable, Category="Inventory - Crafting") bool isCurrentlyProcessing() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") float GetRemainingTime() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") float GetElapsedTime() const;
};
