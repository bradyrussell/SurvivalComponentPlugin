// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVALCOMPONENTSPLUGIN_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// text shown when hovering over the Interactable
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction") FText GetInteractionText();

	// for instance, lock, unlock, search
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction") FName GetInteractionType();

	//interact with the object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction") bool Interact(APawn* InteractingPawn = nullptr, APlayerController* InteractingController = nullptr, int32 Flag = 0);

};
