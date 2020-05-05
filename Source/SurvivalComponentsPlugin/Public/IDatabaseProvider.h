// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "IDatabaseProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIDatabaseProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVALCOMPONENTSPLUGIN_API IIDatabaseProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Database Provider Interface")  UDataTable* GetItemDefinitions();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Database Provider Interface")  UDataTable* GetRecipeDefinitions();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Database Provider Interface")  UDataTable* GetLootDefinitions();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Database Provider Interface")  UDataTable* GetBuildingDefinitions();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Database Provider Interface")  UDataTable* GetShelterDefinitions();
};
