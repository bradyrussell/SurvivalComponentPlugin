// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDeath, class AActor*, KillerActor, class AController*, KillerController, float, Damage, TSubclassOf<UDamageType> , DamageType,const FText, Weapon);

//todo add function ApplyHealthOverTime(float Duration, float TotalAmount)


UCLASS( ClassGroup=(Survival), meta=(BlueprintSpawnableComponent) )
	class SURVIVALCOMPONENTSPLUGIN_API USurvivalComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USurvivalComponent();

	UPROPERTY(BlueprintAssignable, Category="Survival - On Death Event") FOnDeath OnDeath_Event;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float MaxHealth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float Health;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float HealthRegenerationPerSecond;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool bIsRegeneratingHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float MaxStamina;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float Stamina;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float StaminaRegenerationPerSecond;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool bIsRegeneratingStamina;

	// percent damage reduction
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) float ArmorPercentage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool Heal(float Amount);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool TakeDamage(float Damage, AActor* Cause, AController* Instigator, TSubclassOf<UDamageType>  DamageType, bool bIgnoreArmor = false, const FText Weapon = FText());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool DeductStamina(float Amount, bool bAllowNegative = false);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool RestoreStamina(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetMaxHealth(float newMaxHealth) { this->MaxHealth = newMaxHealth; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetHealthRegenerationPerSecond(float newHealthRegenerationPerSecond) {
		this->HealthRegenerationPerSecond = newHealthRegenerationPerSecond;
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIsRegeneratingHealth(bool newbIsRegeneratingHealth) { this->bIsRegeneratingHealth = newbIsRegeneratingHealth; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetMaxStamina(float newMaxStamina) { this->MaxStamina = newMaxStamina; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetStaminaRegenerationPerSecond(float newStaminaRegenerationPerSecond) {
		this->StaminaRegenerationPerSecond = newStaminaRegenerationPerSecond;
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetIsRegeneratingStamina(bool newbIsRegeneratingStamina) { this->bIsRegeneratingStamina = newbIsRegeneratingStamina; }
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetArmorPercentage(float newArmorPercentage) { this->ArmorPercentage = newArmorPercentage; }

	UFUNCTION(BlueprintPure) float GetHealthPercentage() const;
	UFUNCTION(BlueprintPure) float GetStaminaPercentage() const;

};
