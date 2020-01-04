// Fill out your copyright notice in the Description page of Project Settings.

#include "DatabaseProvider.h"
#include "InventoryStructs.h"
#include "Kismet/GameplayStatics.h"
#include "IDatabaseProvider.h"


FItemDefinition UDatabaseProvider::GetItemDefinition(UObject* WorldContextObject, FName item) {
	FString context = FString();

	const auto ItemDB = IIDatabaseProvider::Execute_GetItemDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(ItemDB);
	
	return *ItemDB->FindRow<FItemDefinition>(item, context);
}

FProcessingRecipe UDatabaseProvider::GetRecipeDefinition(UObject* WorldContextObject, FName item) {
	FString context = FString();

	const auto DB = IIDatabaseProvider::Execute_GetRecipeDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);
	
	return *DB->FindRow<FProcessingRecipe>(item, context);
}

FLootDefinition UDatabaseProvider::GetLootDefinition(UObject* WorldContextObject, FName LootTable) {
	FString context = FString();

	const auto DB = IIDatabaseProvider::Execute_GetLootDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);
	
	return *DB->FindRow<FLootDefinition>(LootTable, context);
}

TArray<FItemStack> UDatabaseProvider::CalculateLootDrop(UObject* WorldContextObject, FName LootTable) {
	auto LootDef = GetLootDefinition(WorldContextObject, LootTable);

	TArray<FItemStack> Loot;
	Loot.Append(LootDef.Guarantees);

	float roll = FMath::FRandRange(0.0f,1.0f);

	for (auto&elem:LootDef.Possibilities) {
		roll -= elem.ChancePercent;
		if(roll <= 0.f) { // this is the rolled drop
			Loot.Append(elem.Items);
			return Loot;
		}
	}//rolled nothing
	return Loot;
}

TMap<FName, FProcessingRecipe> UDatabaseProvider::GetAllRecipesOfType(UObject* WorldContextObject, ECraftingType Type) {
	FString context = FString();

	const auto DB = IIDatabaseProvider::Execute_GetRecipeDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);

	TMap<FName, FProcessingRecipe> output;

	for(auto &elem:DB->GetRowMap()) {
		FProcessingRecipe* recipe = reinterpret_cast<FProcessingRecipe*>(elem.Value); 
		if(recipe->RecipeType == Type) output.Emplace(elem.Key, *recipe);
	}//todo optimize

	return output; // if inventory.hasAll output.Input

}

TMap<FName, FProcessingRecipe> UDatabaseProvider::GetAllRecipesForCraftingComponent(UObject* WorldContextObject, UCraftingInventoryComponent* Inventory) {
	FString context = FString();

	const auto DB = IIDatabaseProvider::Execute_GetRecipeDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);

	TMap<FName, FProcessingRecipe> output;

	for(auto &elem:DB->GetRowMap()) {
		FProcessingRecipe* recipe = reinterpret_cast<FProcessingRecipe*>(elem.Value);
		if(recipe->RecipeType == Inventory->Type && Inventory->hasItems(recipe->InputItems)) output.Emplace(elem.Key, *recipe);
	}//todo optimize

	return output; 
}


int32 UDatabaseProvider::ItemToIndex(UObject* WorldContextObject, FName Item) {
	// auto instance = Cast<IIDatabaseProvider>(UGameplayStatics::GetGameInstance(WorldContextObject));
	// check(instance);
	// const auto ItemDB = instance->Execute_GetItemDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	// check(ItemDB);
	const auto ItemDB = IIDatabaseProvider::Execute_GetItemDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(ItemDB);
	
	return ItemDB->GetRowNames().Find(Item)+1;
}

FName UDatabaseProvider::IndexToItem(UObject* WorldContextObject, int32 Index) {
	//auto instance = /*Cast<IIDatabaseProvider>*/(UGameplayStatics::GetGameInstance(WorldContextObject));
	//check(instance);
	const auto ItemDB = IIDatabaseProvider::Execute_GetItemDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(ItemDB);


	auto Names = ItemDB->GetRowNames();
	if(!Names.IsValidIndex(Index-1)) return NAME_None;
	return Names[Index-1];
}

int32 UDatabaseProvider::RecipeToIndex(UObject* WorldContextObject, FName Recipe) {
	const auto DB = IIDatabaseProvider::Execute_GetRecipeDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);

	return DB->GetRowNames().Find(Recipe)+1;
}

FName UDatabaseProvider::IndexToRecipe(UObject* WorldContextObject, int32 Index) {
	const auto DB = IIDatabaseProvider::Execute_GetRecipeDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);

	auto Names = DB->GetRowNames();
	if(!Names.IsValidIndex(Index-1)) return NAME_None;
	return Names[Index-1];
}

int32 UDatabaseProvider::BuildingUnitToIndex(UObject* WorldContextObject, TSubclassOf<ABuildingUnitBase> BuildingUnitClass) {
	const auto DB = IIDatabaseProvider::Execute_GetBuildingDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);
	const FString context;
	
	auto keys = DB->GetRowNames();

	int32 outIndex = -1;
	
	DB->ForeachRow<FBuildingUnitDefinition>(context, [keys, BuildingUnitClass, &outIndex](const FName& Key, const FBuildingUnitDefinition& Value) {
		if(Value.BuildingUnitClass == BuildingUnitClass) outIndex = keys.Find(Key);
	});

	return outIndex;
}

TSubclassOf<ABuildingUnitBase> UDatabaseProvider::IndexToBuildingUnit(UObject* WorldContextObject, int32 Index) {
		const auto DB = IIDatabaseProvider::Execute_GetBuildingDefinitions(UGameplayStatics::GetGameInstance(WorldContextObject));
	check(DB);

	const FString context;

	TArray<FBuildingUnitDefinition*> out;
	
	DB->GetAllRows<FBuildingUnitDefinition>(context, out);

	return out[Index]->BuildingUnitClass;
}
