// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSurvivalComponent.h"
#include "Net/UnrealNetwork.h"


UPlayerSurvivalComponent::UPlayerSurvivalComponent() {
	bIsDeductingHunger = true;
	bIsDeductingThirst = true;
	bIsDeductingOxygen = true;
}

void UPlayerSurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsDeductingHunger)
		if (!DeductHunger(HungerDeductedPerSecond * DeltaTime)){
			OnStarvingTick(DeltaTime);
			if(StarvingTick_Event.IsBound()) StarvingTick_Event.Broadcast(DeltaTime);
		}

	if (bIsDeductingThirst)
		if (!DeductThirst(ThirstDeductedPerSecond * DeltaTime)){
			OnDehydratedTick(DeltaTime);
			if(DehydratedTick_Event.IsBound()) DehydratedTick_Event.Broadcast(DeltaTime);
		}

	if (bIsDeductingOxygen)
		if (!DeductOxygen(OxygenDeductedPerSecond * DeltaTime)){
			OnSuffocatingTick(DeltaTime);
			if(SuffocatingTick_Event.IsBound()) SuffocatingTick_Event.Broadcast(DeltaTime);
		}
}

bool UPlayerSurvivalComponent::RestoreHunger(float Amount) {
	if (Hunger == MaxHunger)
		return false;
	Hunger = FMath::Min(MaxHunger, Hunger + Amount);
	return true;
}

bool UPlayerSurvivalComponent::RestoreThirst(float Amount) {
	if (Thirst == MaxThirst)
		return false;
	Thirst = FMath::Min(MaxThirst, Thirst + Amount);
	return true;
}

bool UPlayerSurvivalComponent::RestoreOxygen(float Amount) {
	if (Oxygen == MaxOxygen)
		return false;
	Oxygen = FMath::Min(MaxOxygen, Oxygen + Amount);
	return true;
}

bool UPlayerSurvivalComponent::DeductHunger(float Amount) {
	if (Amount <= Hunger) {
		Hunger -= FMath::Max(0.f, Amount);
		return true;
	}
	return false;
}

bool UPlayerSurvivalComponent::DeductThirst(float Amount) {
	if (Amount <= Thirst) {
		Thirst -= FMath::Max(0.f, Amount);
		return true;
	}
	return false;
}

bool UPlayerSurvivalComponent::DeductOxygen(float Amount) {
	if (Amount <= Oxygen) {
		Oxygen -= FMath::Max(0.f, Amount);
		return true;
	}
	return false;
}

float UPlayerSurvivalComponent::GetHungerPercentage() const { return Hunger / MaxHunger; }

float UPlayerSurvivalComponent::GetThirstPercentage() const { return Thirst / MaxThirst; }

float UPlayerSurvivalComponent::GetOxygenPercentage() const { return Oxygen / MaxOxygen; }

void UPlayerSurvivalComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(UPlayerSurvivalComponent, Hunger);
	DOREPLIFETIME(UPlayerSurvivalComponent, MaxHunger);
	DOREPLIFETIME(UPlayerSurvivalComponent, HungerDeductedPerSecond);
	DOREPLIFETIME(UPlayerSurvivalComponent, bIsDeductingHunger);

	DOREPLIFETIME(UPlayerSurvivalComponent, Thirst);
	DOREPLIFETIME(UPlayerSurvivalComponent, MaxThirst);
	DOREPLIFETIME(UPlayerSurvivalComponent, ThirstDeductedPerSecond);
	DOREPLIFETIME(UPlayerSurvivalComponent, bIsDeductingThirst);

	DOREPLIFETIME(UPlayerSurvivalComponent, Oxygen);
	DOREPLIFETIME(UPlayerSurvivalComponent, MaxOxygen);
	DOREPLIFETIME(UPlayerSurvivalComponent, OxygenDeductedPerSecond);
	DOREPLIFETIME(UPlayerSurvivalComponent, bIsDeductingOxygen);

	DOREPLIFETIME(USurvivalComponent, MaxHealth);
	DOREPLIFETIME(USurvivalComponent, Health);
	DOREPLIFETIME(USurvivalComponent, HealthRegenerationPerSecond);
	DOREPLIFETIME(USurvivalComponent, bIsRegeneratingHealth);

	DOREPLIFETIME(USurvivalComponent, Stamina);
	DOREPLIFETIME(USurvivalComponent, MaxStamina);
	DOREPLIFETIME(USurvivalComponent, StaminaRegenerationPerSecond);
	DOREPLIFETIME(USurvivalComponent, bIsRegeneratingStamina);

	DOREPLIFETIME(USurvivalComponent, ArmorPercentage);
}
