// Fill out your copyright notice in the Description page of Project Settings.


#include "ShelterUnitBase.h"
#include "DatabaseProvider.h"
#include "ShelterBuilderComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AShelterUnitBase::AShelterUnitBase() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ShelterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShelterMesh"));
	SetRootComponent(ShelterMesh);

	//SU_Type = FName(*this->GetName());
}

// Called when the game starts or when spawned
void AShelterUnitBase::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AShelterUnitBase::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

FName AShelterUnitBase::getNearestSocket(FVector location) const {
	float distance = INFINITY;
	FName closest = NAME_None;

	auto sockets = ShelterMesh->GetAllSocketNames();
	for (auto& socket : sockets) {
		const float tempDist = FVector::DistSquared(location, ShelterMesh->GetSocketLocation(socket));
		if (distance > tempDist) {
			closest = socket;
			distance = tempDist;
		}
	}

	return closest;
}

FName AShelterUnitBase::getNearestSocketOfType(FVector location, FString type) const {
	float distance = INFINITY;
	FName closest = NAME_None;

	auto sockets = ShelterMesh->GetAllSocketNames();
	for (auto& socket : sockets) {
		float tempDist = FVector::DistSquared(location, ShelterMesh->GetSocketLocation(socket));
		FString socket_name = socket.ToString();
		int32 Index = 0;
		if (socket_name.FindChar('_', Index)) { socket_name = socket_name.Left(Index); }

		if (socket_name.Equals(type, ESearchCase::IgnoreCase) && distance > tempDist) {
			closest = socket;
			distance = tempDist;
		}
	}

	return closest;
}


int32 AShelterUnitBase::getSocketIndex(FName socket) const {
	const auto sockets = ShelterMesh->GetAllSocketNames();
	return sockets.Find(socket);
}

void AShelterUnitBase::CheckStability() {
	auto ShelterDef = UDatabaseProvider::GetShelterUnitDefinitionByIndex(this,UDatabaseProvider::ShelterUnitToIndex(this,this->GetClass()));
	if(!ShelterDef.bRequiresSocket) return;
	
	FHitResult OutHit;
	FVector Origin, Extent;

	GetActorBounds(true, Origin, Extent);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Origin, Origin - FVector(0, 0, Extent.Z + 5), ECC_Visibility);

	if (bHit) {
		AShelterUnitBase* SU = Cast<AShelterUnitBase>(OutHit.Actor);
		if (SU) {
			if (SU->IsPendingKill())
				DestroyFromInstability();
		}
		else { DestroyFromInstability(); }
	}
	else { DestroyFromInstability(); }

}

void AShelterUnitBase::DestroyFromInstability() {
	auto ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Add(this);
	
	auto SocketNames = ShelterMesh->GetAllSocketNames();

	this->Destroy();

	for (auto& elem : SocketNames) {
		
		auto sLocation = ShelterMesh->GetSocketLocation(elem);
		TArray<AActor*> OutActors;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		UShelterBuilderComponent::BoxOverlapActors(this,sLocation,GetActorQuat(),FVector(5,5,5), TArray<TEnumAsByte<EObjectTypeQuery>>(),nullptr,ActorsToIgnore,OutActors);
		VisualizeStabilityCheck(sLocation,FVector(5,5,5), GetActorRotation());
		
		for(auto&actor:OutActors){
			AShelterUnitBase* SU = Cast<AShelterUnitBase>(actor);
			if (SU && SU != this) { SU->CheckStability(); }
		} 
	}
}
