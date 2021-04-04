#pragma once

#include "CoreMinimal.h"
#include "ItemEffectBase.h"
#include "Engine/Texture2D.h"
#include "Engine/DataTable.h"
#include "GameplayEffect.h"
#include "InventoryStructs.generated.h"

USTRUCT(BlueprintType)
	struct FItemStack {
	GENERATED_BODY()
	FItemStack()
		: Amount(0), Item(NAME_None), Data("") { 
	}

	/*FItemStack(FName Item, int32 Amount)
		: Amount(Amount),Item(Item), Data("") {
	}*/

	FItemStack(FName Item, int32 Amount, FString Data)
		: Amount(Amount),Item(Item), Data(Data) {
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category="Item Stack") int32 Amount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category="Item Stack") FName Item;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category="Item Stack") FString Data;

	/*UFUNCTION(BlueprintPure) not allowed*/
	bool IsEmpty() const { return Item == NAME_None || Amount == 0; }
	bool HasData() const { return !Data.IsEmpty(); }
	FORCEINLINE bool operator<(const FItemStack &Other) const
	{
		if(this->Item == Other.Item) return this->Amount > Other.Amount;
		return this->Item .LexicalLess(Other.Item); // this is necessary to ensure A->Z, but 255->0 ordering when sorted
	}

	FORCEINLINE bool operator==(const FItemStack &Other) const
	{
		return this->Item == Other.Item && this->Amount == Other.Amount;
	}

};

UENUM(BlueprintType)
	enum class ECraftingType : uint8 {
	CT_None UMETA(DisplayName="None"),
	CT_Crafting UMETA(DisplayName="Crafting"),
	CT_Cooking UMETA(DisplayName="Cooking"),
	CT_Advanced UMETA(DisplayName="Advanced")
};

USTRUCT(BlueprintType)
	struct FItemDefinition : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") FText DisplayName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Item") int32 MaxStack = 256;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Consumable or Equipable") bool bIsConsumable; 
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Consumable or Equipable") TArray<uint8> ValidEquipmentSlots;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Item") int32 Value;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") FText DisplayCategory;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") FText DisplayDescription;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") UTexture2D* DisplayIcon;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") UStaticMesh* DisplayMesh;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") UMaterialInterface* MeshMaterialOverride;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Consumable or Equipable") TSubclassOf<UGameplayEffect> ItemEffect;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Consumable or Equipable") USoundBase* ItemSound;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Consumable or Equipable") float ItemEffectMagnitude = 1.0f;

};




USTRUCT(BlueprintType)
	struct FLootDrop  {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Loot") float ChancePercent = 0.5;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Loot") TArray<FItemStack> Items;

};

USTRUCT(BlueprintType)
	struct FLootDefinition : public FTableRowBase {
	GENERATED_BODY()

	// Chances should add up to or less than one. If less than one, remaining chance drops nothing
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Loot Table")TArray<FLootDrop> Possibilities;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Loot Table")TArray<FItemStack> Guarantees; 

};


USTRUCT(BlueprintType)
	struct FProcessingRecipe : public FTableRowBase {
	GENERATED_BODY()


	FProcessingRecipe() = default;

	FProcessingRecipe(ECraftingType RecipeType, const TArray<FItemStack> InputItems, const TArray<FItemStack> OutputItems, const float ProcessingDuration)
		: RecipeType(RecipeType), ProcessingDuration(ProcessingDuration),
		  InputItems(InputItems), OutputItems(OutputItems) {
	}

	FProcessingRecipe(
		ECraftingType RecipeType,
		const TArray<FItemStack> InputItems,
		const TArray<FItemStack> OutputItems,
		const float ProcessingDuration,
		bool bRequiresFuel = false,
		float FuelRequired = 0.0,
		bool bHasVariableYield = false,
		const FVector2D VariableYieldRange = FVector2D(0, 0),
		bool bRequiresCatalyst = false,
		const TArray<FItemStack> CatalystItems = TArray<FItemStack>()
	)
		: RecipeType(RecipeType), ProcessingDuration(ProcessingDuration),
		  bRequiresFuel(bRequiresFuel),
		  bRequiresCatalyst(bRequiresCatalyst),
		  bHasVariableYield(bHasVariableYield),
		  InputItems(InputItems),
		  OutputItems(OutputItems),
		  VariableYieldRange(VariableYieldRange),
		  CatalystItems(CatalystItems),
		  FuelRequired(FuelRequired) {
	}

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") ECraftingType RecipeType = ECraftingType::CT_None;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") FText DisplayName;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") FText DisplayDescription;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Display") UTexture2D* DisplayIcon;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") float ProcessingDuration = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") bool bRequiresFuel = false;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") bool bRequiresCatalyst = false;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") bool bHasVariableYield = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") TArray<FItemStack> InputItems;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") TArray<FItemStack> OutputItems;

	// the min - max percentages which the output will be multiplied by
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") FVector2D VariableYieldRange;

	// if RequiresCatalyst, these items must be present as input but wont be consumed
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") TArray<FItemStack> CatalystItems;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Recipe") float FuelRequired = 0.f;
};
