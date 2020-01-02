// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemEffectBase.h"
#include "GameFramework/DamageType.h"
#include "EquipmentItemEffectBase.generated.h"

class AController;
/**
 * 
 */
UCLASS(Blueprintable, Blueprintable)
class SURVIVALCOMPONENTSPLUGIN_API UEquipmentItemEffectBase : public UItemEffectBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float ArmorPercentage;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly) FText EquipmentText;

	//return true if the item should be consumed
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) bool OnItemEquipped(APawn* Cause = nullptr, AController* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) bool OnItemUnequipped(APawn* Cause = nullptr, AController* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) bool OnEquipmentTick(float DeltaTime, APawn* Cause = nullptr, AController* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) bool OnEquipmentHit(float Damage, AActor* DamageCause, AController* DamageInstigator, TSubclassOf<UDamageType>  DamageType, APawn* Wearer = nullptr, AController* WearerController = nullptr);
};
