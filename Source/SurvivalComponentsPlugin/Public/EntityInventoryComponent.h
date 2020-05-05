// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "EquipmentItemEffectBase.h"
#include "EntityInventoryComponent.generated.h"

class AController;
/**
 * 
 */
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) , Blueprintable)
class SURVIVALCOMPONENTSPLUGIN_API UEntityInventoryComponent : public UBaseInventoryComponent
{
	GENERATED_BODY()

public:
	UEntityInventoryComponent();

protected:
	void BeginPlay() override;

public:
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated, Category="Inventory - Equipment") int32 NumberEquipmentSlots; // not  savegame because it should be const
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated, Category="Inventory - Equipment", SaveGame) TArray<FItemStack> EquipmentSlots;

	//this array is aligned with equipmentslots
	UPROPERTY(BlueprintReadWrite) TArray<UEquipmentItemEffectBase*> CurrentEquipmentEffects;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool ConsumeItem(int32 Slot, APawn* Cause = nullptr, AController* Instigator = nullptr, float MagnitudeMultiplier = 1.0);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool EquipItem(int32 Slot, uint8 EquipmentSlot, APawn* Cause = nullptr, AController* Instigator = nullptr);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool UnequipItem(int32 Slot, uint8 EquipmentSlot, APawn* Cause = nullptr, AController* Instigator = nullptr);

	UFUNCTION(BlueprintCallable) bool OnEquipmentHit(float Damage, AActor* DamageCause, AController* DamageInstigator, TSubclassOf<UDamageType>  DamageType, APawn* Wearer = nullptr, AController* WearerController = nullptr);
};
