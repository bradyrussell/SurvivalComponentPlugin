// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "BuildingUnitBase.h"
#include "ShelterUnitBase.generated.h"


UCLASS()
class SURVIVALCOMPONENTSPLUGIN_API AShelterUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShelterUnitBase();


	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* ShelterMesh;
	//UPROPERTY(BlueprintReadOnly, EditAnywhere) FName SU_Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Replicated) FString Creator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, ReplicatedUsing=OnRep_Health) int32 Health = 1000;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame) int32 MaxHealth = 1000;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//returns NAME_None on failure
	UFUNCTION(BlueprintCallable) FName getNearestSocket(FVector location) const;

	//returns NAME_None on failure
	UFUNCTION(BlueprintCallable) FName getNearestSocketOfType(FVector location, FString type) const;

	UFUNCTION(BlueprintCallable, BlueprintPure) int32 getSocketIndex(FName socket) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void CheckStability();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void DestroyFromInstability();

	UFUNCTION(BlueprintImplementableEvent) void VisualizeStabilityCheck(FVector Origin, FVector Extent, FRotator Rotation);

	UFUNCTION(BlueprintImplementableEvent) void OnRep_Health();

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
