// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

#include <string>
#include "ArchiveOverloads.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere) FString Creator;
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
};
