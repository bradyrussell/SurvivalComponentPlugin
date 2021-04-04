// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ContainerInventoryComponent.h"
#include "Engine/DataTable.h"
#include "CraftingInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBeginProcessingRecipe, FName, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFinishProcessingRecipe, FName, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFailedProcessingRecipe, FName, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQueuedProcessingRecipe, FName, Recipe);

/**
 * 
 */
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
	class SURVIVALCOMPONENTSPLUGIN_API UCraftingInventoryComponent : public UContainerInventoryComponent {
	GENERATED_BODY()
public:

	UDataTable* GetRecipesDataTable() const;
	FProcessingRecipe* GetRecipeDefinition(FName Recipe) const;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated, Category="Inventory - Crafting") ECraftingType Type;

	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FBeginProcessingRecipe BeginProcessingRecipe_Event;
	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FFinishProcessingRecipe FinishProcessingRecipe_Event;
	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FFailedProcessingRecipe FailedProcessingRecipe_Event;
	UPROPERTY(BlueprintAssignable, Category="Inventory - Crafting Events") FQueuedProcessingRecipe QueuedProcessingRecipe_Event;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Replicated, Category="Inventory - Crafting") TArray<FName> ProcessingQueue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, SaveGame, Category="Inventory - Crafting") float Fuel = 0.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Inventory - Crafting") float MaximumFuel = 10000.f;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Replicated, Category="Inventory - Crafting") bool bIsCurrentlyProcessing = false;

	UPROPERTY(BlueprintReadWrite, Category="Inventory - Crafting") FTimerHandle CurrentJob_Timer;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Replicated, Category="Inventory - Crafting") FName CurrentJob;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") bool QueueRecipe(FName InRecipe);

	// will begin working through the queued recipes
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") bool BeginProcessing();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") FName PeekNextRecipe() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") bool AddFuel(float Amount);

	UFUNCTION(BlueprintCallable) TArray<FName> GetAvailableRecipes();
private:
	//called when a recipe finishes. 
	void EndProcessing();

	UFUNCTION(NetMulticast, Reliable) void Multi_BeginProcessingRecipe(FName Recipe);
	UFUNCTION(NetMulticast, Reliable) void Multi_FinishProcessingRecipe(FName Recipe);
	UFUNCTION(NetMulticast, Reliable) void Multi_FailedProcessingRecipe(FName Recipe);
	UFUNCTION(NetMulticast, Reliable) void Multi_QueuedProcessingRecipe(FName Recipe);

public:
	//interrupt crafting, does not clear queue
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") void CancelProcessing();

	// clear queue. does not cancel in progress recipe
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") void ClearQueue();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") void CancelAndClearQueue();

	UFUNCTION(BlueprintCallable, Category="Inventory - Crafting") bool IsCurrentlyProcessing() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") float GetRemainingTime() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Crafting") float GetElapsedTime() const;
};
