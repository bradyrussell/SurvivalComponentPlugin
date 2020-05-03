// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USurvivalComponent::USurvivalComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	SetIsReplicatedByDefault(true);

	Health = MaxHealth;
	Stamina = MaxStamina;

	bIsRegeneratingHealth = false;
	bIsRegeneratingStamina = true;
}


// Called when the game starts
void USurvivalComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void USurvivalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// do everything based on delta t

	if(bIsRegeneratingHealth) Heal(HealthRegenerationPerSecond * DeltaTime);
	if(bIsRegeneratingStamina) RestoreStamina(StaminaRegenerationPerSecond * DeltaTime);
}

bool USurvivalComponent::Heal(float Amount) {
	if(Health == MaxHealth) return false;
	Health = FMath::Min(MaxHealth, Health + Amount);
	return true;
}


bool USurvivalComponent::TakeDamage(float Damage, AActor* Cause, AController* Instigator, TSubclassOf<UDamageType> DamageType, bool bIgnoreArmor, const FText Weapon) {
	float RealDamage = bIgnoreArmor ? Damage : Damage - (Damage * ArmorPercentage);

	Health -= FMath::Max(0.f, RealDamage); // no negative damage allowed

	if(Health <= 0 && OnDeath_Event.IsBound()) {
		OnDeath_Event.Broadcast(Cause, Instigator, Damage, DamageType, Weapon);
		return false;
	}

	return true;
}

bool USurvivalComponent::DeductStamina(float Amount, bool bAllowNegative) {
	if (Amount <= Stamina || bAllowNegative) {
		Stamina -= FMath::Max(0.f, Amount);
		return true;
	}
	return false;
}

bool USurvivalComponent::RestoreStamina(float Amount) {
	if(Stamina == MaxStamina) return false;
	Stamina = FMath::Min(MaxStamina, Stamina + Amount);
	return true;
}

float USurvivalComponent::GetHealthPercentage() const {
	return Health / MaxHealth;
}

float USurvivalComponent::GetStaminaPercentage() const {
	return Stamina / MaxStamina;
}

void USurvivalComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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