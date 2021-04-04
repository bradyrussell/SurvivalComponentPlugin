// Fill out your copyright notice in the Description page of Project Settings.
#include "BuildingHologram.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ABuildingHologram::ABuildingHologram()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void ABuildingHologram::SetMaterial(UMaterialInterface* aMaterial) {
	if(aMaterial != Material) {
		for (int i = 0; i < Mesh->GetNumMaterials(); ++i) {
			Mesh->SetMaterial(i, aMaterial);
		}
	}
}

// Called when the game starts or when spawned
void ABuildingHologram::BeginPlay()
{
	Super::BeginPlay();
	
}

