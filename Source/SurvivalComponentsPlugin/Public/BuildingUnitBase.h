/*
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "BuildSystemStructs.h"
#include "ArchiveOverloads.h"
#include <string>
#include "BuildingUnitBase.generated.h"

#define BASEFILE TEXT("C:/Users/Admin/Documents/Unreal Projects/BuildSystem/Base.zcb")

UCLASS()
class SURVIVALCOMPONENTSPLUGIN_API ABuildingUnitBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingUnitBase();

	// this is aligned with the getSocketNames array indices
	UPROPERTY(BlueprintReadOnly) TArray<ABuildingUnitBase*> socketedChildren;
	UPROPERTY(BlueprintReadOnly) TArray<FUnsocketedAttachment> unsocketedChildren;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UStaticMeshComponent* BuildingMesh;
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FName BU_Type;
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

	//split a socket FName into its constituent parts
	UFUNCTION(BlueprintCallable, BlueprintPure) static FBuildingSocket parseSocketName(FName socket);

	UFUNCTION(BlueprintCallable) void AddSocketedChild(FName socket, ABuildingUnitBase* child);

	/*UFUNCTION(BlueprintCallable) #1#void AddSocketedChild(int32 socket_index, ABuildingUnitBase* child);

	UFUNCTION(BlueprintCallable) void AddUnsocketedChild(FTransform attachment, ABuildingUnitBase* child);

	UFUNCTION(BlueprintCallable, BlueprintPure) int32 getSocketIndex(FName socket) const;

	UFUNCTION(BlueprintCallable) ABuildingUnitBase* RecursiveGetRoot();

	static std::string RecursiveSerialize(ABuildingUnitBase* RootBuildingUnitBase);

	UFUNCTION(BlueprintCallable) static FString NewSerializeTest(ABuildingUnitBase* RootBuildingUnitBase);

	UFUNCTION(BlueprintCallable) static bool SaveToFile(ABuildingUnitBase* RootBuildingUnitBase);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject")) static bool LoadFromFile(UObject* WorldContextObject, FTransform RootLocation);
	
	FSerializedBuildingUnit SerializeToStruct();

	static ABuildingUnitBase* DeserializeFromStruct(UObject* WorldContextObject, FSerializedBuildingUnit SerializedBuildingUnit, FTransform transform = FTransform());

	UFUNCTION(BlueprintPure, Category="Building Unit Save", meta = (WorldContext = "WorldContextObject")) static ABuildingUnitBase* DeserializeBuildingFromStruct(UObject* WorldContextObject, FSerializedBuilding SerializedBuilding);
	

};
*/
