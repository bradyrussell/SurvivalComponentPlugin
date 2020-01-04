// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
//#include "Factories/CompositeDataTableFactory.h"
#include "InventoryStructs.h"
#include "BaseInventoryComponent.h"
#include "CraftingInventoryComponent.h"
#include "BuildingUnitBase.h"
#include "DatabaseProvider.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALCOMPONENTSPLUGIN_API UDatabaseProvider : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	//UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static int32 GetMaxStackForItem(UObject* WorldContextObject, FName item);

	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static FItemDefinition GetItemDefinition(UObject* WorldContextObject, FName item);
	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static FProcessingRecipe GetRecipeDefinition(UObject* WorldContextObject, FName item);

	UFUNCTION(BlueprintPure, Category="Inventory", meta = (WorldContext = "WorldContextObject")) static FLootDefinition GetLootDefinition(UObject* WorldContextObject, FName LootTable);
	UFUNCTION(BlueprintPure, Category="Inventory", meta = (WorldContext = "WorldContextObject")) static TArray<FItemStack> CalculateLootDrop(UObject* WorldContextObject, FName LootTable);

	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static TMap<FName, FProcessingRecipe> GetAllRecipesOfType(
		UObject* WorldContextObject,
		ECraftingType Type);

	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static TMap<FName, FProcessingRecipe> GetAllRecipesForCraftingComponent(
		UObject* WorldContextObject,
		UCraftingInventoryComponent* Inventory);

	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static int32 ItemToIndex(UObject* WorldContextObject, FName Item);
	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static FName IndexToItem(UObject* WorldContextObject, int32 Index);

	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static int32 RecipeToIndex(UObject* WorldContextObject, FName Recipe);
	UFUNCTION(BlueprintPure, Category="Inventory Data", meta = (WorldContext = "WorldContextObject")) static FName IndexToRecipe(UObject* WorldContextObject, int32 Index);

	UFUNCTION(BlueprintPure, Category="Building Unit Data", meta = (WorldContext = "WorldContextObject")) static int32 BuildingUnitToIndex(UObject* WorldContextObject, TSubclassOf<ABuildingUnitBase> BuildingUnitClass);
	UFUNCTION(BlueprintPure, Category="Building Unit Data", meta = (WorldContext = "WorldContextObject")) static TSubclassOf<ABuildingUnitBase> IndexToBuildingUnit(UObject* WorldContextObject, int32 Index);
};
