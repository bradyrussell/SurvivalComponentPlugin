// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SurvivalComponent.h"
#include "PlayerSurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStarvingTick, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDehydratedTick, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSuffocatingTick, float, DeltaTime);

/**
 * 
 */
UCLASS( ClassGroup=(Survival), meta=(BlueprintSpawnableComponent) )
	class SURVIVALCOMPONENTSPLUGIN_API UPlayerSurvivalComponent : public USurvivalComponent {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="Survival - On Starving Tick") FOnStarvingTick StarvingTick_Event; 
	UPROPERTY(BlueprintAssignable, Category="Survival - On Dehydrated Tick") FOnDehydratedTick DehydratedTick_Event; 
	UPROPERTY(BlueprintAssignable, Category="Survival - On Suffocating Tick") FOnSuffocatingTick SuffocatingTick_Event; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float MaxHunger;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float Hunger;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float HungerDeductedPerSecond;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool bIsDeductingHunger;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float MaxThirst;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float Thirst;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float ThirstDeductedPerSecond;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool bIsDeductingThirst;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float MaxOxygen;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float Oxygen;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float OxygenDeductedPerSecond;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool bIsDeductingOxygen;


public:

	UPlayerSurvivalComponent();
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool RestoreHunger(float Amount);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool RestoreThirst(float Amount);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool RestoreOxygen(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool DeductHunger(float Amount);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool DeductThirst(float Amount);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool DeductOxygen(float Amount);

	UFUNCTION(BlueprintImplementableEvent) void OnStarvingTick(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent) void OnDehydratedTick(float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent) void OnSuffocatingTick(float DeltaTime);

	UFUNCTION(BlueprintPure) float GetHungerPercentage() const;
	UFUNCTION(BlueprintPure) float GetThirstPercentage() const;
	UFUNCTION(BlueprintPure) float GetOxygenPercentage() const;
};
