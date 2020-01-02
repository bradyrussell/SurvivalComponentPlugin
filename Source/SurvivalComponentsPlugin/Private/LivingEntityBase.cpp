// Fill out your copyright notice in the Description page of Project Settings.


#include "LivingEntityBase.h"

// Sets default values
ALivingEntityBase::ALivingEntityBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALivingEntityBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALivingEntityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

