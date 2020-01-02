// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemEffectBase.h"
#include "ConsumableItemEffectBase.generated.h"

class AController;
/**
 * 
 */
UCLASS(Blueprintable, Blueprintable)
class SURVIVALCOMPONENTSPLUGIN_API UConsumableItemEffectBase : public UItemEffectBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FText ConsumableText;

	//return true if the item should be consumed
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) bool OnItemConsumed(APawn* Cause = nullptr, AController* Instigator = nullptr, float Magnitude = 1.0);
};
