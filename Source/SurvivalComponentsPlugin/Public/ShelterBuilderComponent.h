// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingHologram.h"
#include "ShelterBuilderComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnDeath, class AActor*, KillerActor, class AController*, KillerController, float, Damage, TSubclassOf<UDamageType> , DamageType,const FText, Weapon);

// this belongs on the player controller
UCLASS(Blueprintable, ClassGroup=(ShelterBuilder), meta=(BlueprintSpawnableComponent) )
	class SURVIVALCOMPONENTSPLUGIN_API UShelterBuilderComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UShelterBuilderComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere) float BuildRange;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 SelectedShelterUnitIndex;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) bool bIsBuilding;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) ABuildingHologram* BuildingHologramActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UMaterialInterface* HologramMaterial_OK;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UMaterialInterface* HologramMaterial_Invalid;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsHologramInvalid = false;

	UFUNCTION(BlueprintCallable, Client, Reliable) void Client_ClearHologram();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetSelectedShelterUnit(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void BuildShelterUnit();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void DestroyShelterUnit();

	UFUNCTION(BlueprintImplementableEvent) void OnBuildFailed(FVector Origin, FVector Extent, FRotator Rotation);

public:
	static bool BoxOverlapActors(UObject* WorldContextObject, const FVector BoxPos, const FQuat Rotation, FVector BoxExtent, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, UClass* ActorClassFilter, const TArray<AActor*>& ActorsToIgnore, TArray<AActor*>& OutActors);
	static bool BoxOverlapComponents(UObject* WorldContextObject, const FVector BoxPos, const FQuat Rotation, FVector BoxExtent, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, UClass* ComponentClassFilter, const TArray<AActor*>& ActorsToIgnore, TArray<UPrimitiveComponent*>& OutComponents);
	
	// UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool TakeDamage(float Damage, AActor* Cause, AController* Instigator, TSubclassOf<UDamageType>  DamageType, bool bIgnoreArmor = false, const FText Weapon = FText());
	//


};
