// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SurvivalComponent.h"
#include "BaseInventoryComponent.h"
#include "SurvivalEntity.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USurvivalEntity : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVALCOMPONENTSPLUGIN_API ISurvivalEntity
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Survival Entity")  USurvivalComponent* GetSurvivalComponent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Survival Entity")  UBaseInventoryComponent* GetInventoryComponent();
	
};
