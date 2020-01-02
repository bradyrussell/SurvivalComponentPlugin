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
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction") FText getInteractionText();

	// for instance, lock, unlock, search
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction") FName getInteractionType();

	//interact with the object
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction") bool Interact(APawn* Cause = nullptr, APlayerController* Instigator = nullptr, int32 Flag = 0);

};
