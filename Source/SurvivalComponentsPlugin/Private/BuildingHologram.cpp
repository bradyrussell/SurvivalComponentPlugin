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

// Called when the game starts or when spawned
void ABuildingHologram::BeginPlay()
{
	Super::BeginPlay();
	
}

