// Fill out your copyright notice in the Description page of Project Settings.

#include "ShelterBuilderComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "DatabaseProvider.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "ShelterUnitBase.h"


// Sets default values for this component's properties
UShelterBuilderComponent::UShelterBuilderComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

}


// Called when the game starts
void UShelterBuilderComponent::BeginPlay() {
	Super::BeginPlay();

	// ...

}


// Called every frame
void UShelterBuilderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwnerRole() == ROLE_Authority)
		return;

	if (bIsBuilding) {
		APlayerController* Controller = Cast<APlayerController>(GetOwner());
		if (Controller) {
			APawn* Pawn = Controller->GetPawn();

			if (Pawn) {
				FVector Location;
				FRotator Rotation;

				Pawn->GetActorEyesViewPoint(Location, Rotation);

				const FVector Destination = Location + (Rotation.RotateVector(FVector::ForwardVector) * BuildRange);

				FHitResult HitResult;
				if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Destination, ECC_Visibility)) {
					AShelterUnitBase* ParentShelterUnit = Cast<AShelterUnitBase>(HitResult.Actor);
					auto ShelterInfo = UDatabaseProvider::GetShelterUnitDefinitionByIndex(GetOwner(), SelectedShelterUnitIndex); // should I cache these with a TMAP?

					FVector HologramLocation = HitResult.Location;
					auto HologramRotation = FRotator::ZeroRotator;
					HologramRotation.Yaw += Rotation.Yaw;
					bool hasParent = false;

					if (ParentShelterUnit) {
						auto Socket = ParentShelterUnit->getNearestSocketOfType(HitResult.Location, ShelterInfo.FitsSocket);
						if (Socket != NAME_None) {
							auto Transform = ParentShelterUnit->ShelterMesh->GetSocketTransform(Socket);
							HologramLocation = Transform.GetLocation();
							HologramRotation = Transform.GetRotation().Rotator();
							hasParent = true;
						}
					}
					//else {

					//FVector Origin, Extent;
					//BuildingHologramActor->GetActorBounds(false, Origin, Extent);

					FVector Origin, Extent;

					auto BB = ShelterInfo.ShelterUnitMesh->GetBoundingBox();
					BB.GetCenterAndExtents(Origin, Extent);

					if(ShelterInfo.bOffsetByExtent) {
						HologramLocation += Extent * HitResult.Normal;
					}
					
					Origin += HologramLocation;

					if (IsValid(BuildingHologramActor)) {
						BuildingHologramActor->SetActorLocation(HologramLocation);
						BuildingHologramActor->SetActorRotation(HologramRotation);
					}
					else {
						//spawn building holo
						FActorSpawnParameters Params;
						Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						BuildingHologramActor = GetWorld()->SpawnActor<ABuildingHologram>(ABuildingHologram::StaticClass(), HologramLocation, HologramRotation, Params);

						BuildingHologramActor->Mesh->SetStaticMesh(ShelterInfo.ShelterUnitMesh);

						for (int i = 0; i < BuildingHologramActor->Mesh->GetNumMaterials(); i++) { BuildingHologramActor->Mesh->SetMaterial(i, HologramMaterial_OK); }
					}
					
					
					TArray<AActor*> Ignore, Overlaps;
					
					Ignore.Add(BuildingHologramActor);

					Extent *= .9; // make the hitbox slightly less strict

					UShelterBuilderComponent::BoxOverlapActors(GetOwner(), Origin, HologramRotation.Quaternion(), Extent, TraceObjectTypes, nullptr, Ignore, Overlaps);

					// if(Overlaps.Num() > 0) {
					// 		for(auto&elem:Overlaps) {
					// 			UE_LOG(LogTemp, Warning, TEXT("%s"), *AActor::GetDebugName(elem));
					// 		}
					// 	}
					
					bool bShouldHologramBeValid = Overlaps.Num() == 0 && (!ShelterInfo.bRequiresSocket || ShelterInfo.bRequiresSocket && hasParent);
					if (bIsHologramInvalid != !bShouldHologramBeValid) {
						bIsHologramInvalid = !bShouldHologramBeValid;
					
						UE_LOG(LogTemp, Warning, TEXT("Can build: %hs. %d overlaps, Require socket: %hs, Has socket: %hs"), bShouldHologramBeValid?"true":"false",Overlaps.Num(), ShelterInfo.bRequiresSocket?"true":"false", hasParent?"true":"false");
						for (int i = 0; i < BuildingHologramActor->Mesh->GetNumMaterials(); i++) {
							BuildingHologramActor->Mesh->SetMaterial(i, bIsHologramInvalid ? HologramMaterial_Invalid : HologramMaterial_OK);
						}
					}
					//}
				}
				else { if (IsValid(BuildingHologramActor)) { BuildingHologramActor->Destroy(); } }
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ShelterBuilderComponent owner is not PlayerController, removing!"));
			this->DestroyComponent();
		}
	}
	else {
		if (IsValid(BuildingHologramActor)) { BuildingHologramActor->Destroy(); }
	}

}

void UShelterBuilderComponent::Client_ClearHologram_Implementation() { if (IsValid(BuildingHologramActor)) { BuildingHologramActor->Destroy(); } }

void UShelterBuilderComponent::SetSelectedShelterUnit(int32 Index) {
	SelectedShelterUnitIndex = Index;
	Client_ClearHologram();
}

void UShelterBuilderComponent::BuildShelterUnit() {
	if (GetOwnerRole() != ROLE_Authority)
		return;

	
	if (bIsBuilding) {
		APlayerController* Controller = Cast<APlayerController>(GetOwner());
		if (Controller) {
			APawn* Pawn = Controller->GetPawn();

			if (Pawn) {
				FVector Location;
				FRotator Rotation;

				Pawn->GetActorEyesViewPoint(Location, Rotation);

				const FVector Destination = Location + (Rotation.RotateVector(FVector::ForwardVector) * BuildRange);

				FHitResult HitResult;
				if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Destination, ECC_Visibility)) {
					AShelterUnitBase* ParentShelterUnit = Cast<AShelterUnitBase>(HitResult.Actor);
					auto ShelterInfo = UDatabaseProvider::GetShelterUnitDefinitionByIndex(GetOwner(), SelectedShelterUnitIndex); // should I cache these with a TMAP?

					FVector HologramLocation = HitResult.Location;
					auto HologramRotation = FRotator::ZeroRotator;
					HologramRotation.Yaw += Rotation.Yaw;
					bool hasParent = false;

					if (ParentShelterUnit) {
						auto Socket = ParentShelterUnit->getNearestSocketOfType(HitResult.Location, ShelterInfo.FitsSocket);
						if (Socket != NAME_None) {
							auto Transform = ParentShelterUnit->ShelterMesh->GetSocketTransform(Socket);
							HologramLocation = Transform.GetLocation();
							HologramRotation = Transform.GetRotation().Rotator();
							hasParent = true;
						}
					}

					FVector Origin, Extent;

					auto BB = ShelterInfo.ShelterUnitMesh->GetBoundingBox();
					BB.GetCenterAndExtents(Origin, Extent);

					if(ShelterInfo.bOffsetByExtent) {
						HologramLocation += Extent * HitResult.Normal;
					}
					
					Origin += HologramLocation;

					TArray<AActor*> Ignore, Overlaps;

					Extent *= .9; // make the hitbox slightly less strict

					UShelterBuilderComponent::BoxOverlapActors(GetOwner(), Origin, HologramRotation.Quaternion(), Extent, TraceObjectTypes, nullptr, Ignore, Overlaps);

					if (Overlaps.Num() == 0 && (!ShelterInfo.bRequiresSocket || ShelterInfo.bRequiresSocket && hasParent)) {
						//spawn building 
						FActorSpawnParameters Params;
						Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						GetWorld()->SpawnActor<AShelterUnitBase>(ShelterInfo.ShelterUnitClass, HologramLocation, HologramRotation, Params);
					}
					else {
						//fail
						OnBuildFailed(Origin, Extent, HologramRotation);
						UE_LOG(LogTemp, Warning, TEXT("Failed to build. %d overlaps, Require socket: %hs, Has socket: %hs"), Overlaps.Num(), ShelterInfo.bRequiresSocket?"true":"false", hasParent?"true":"false");
						for(auto&elem:Overlaps) {
							UE_LOG(LogTemp, Warning, TEXT("Overlap %s"), *AActor::GetDebugName(elem));
						}
					}


					//}
				}
				//	else { if (IsValid(BuildingHologramActor)) { BuildingHologramActor->Destroy(); } }
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ShelterBuilderComponent owner is not PlayerController, removing!"));
			this->DestroyComponent();
		}
	}
	


}

void UShelterBuilderComponent::DestroyShelterUnit() {
		if (GetOwnerRole() != ROLE_Authority)
		return;
	
	if (bIsBuilding) {
		APlayerController* Controller = Cast<APlayerController>(GetOwner());
		if (Controller) {
			APawn* Pawn = Controller->GetPawn();

			if (Pawn) {
				FVector Location;
				FRotator Rotation;

				Pawn->GetActorEyesViewPoint(Location, Rotation);

				const FVector Destination = Location + (Rotation.RotateVector(FVector::ForwardVector) * BuildRange);

				FHitResult HitResult;
				if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Destination, ECC_Visibility)) {
					AShelterUnitBase* ShelterUnit = Cast<AShelterUnitBase>(HitResult.Actor);
					if(ShelterUnit)
					ShelterUnit->DestroyFromInstability();
				}
			}
		}
	}
}


void UShelterBuilderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(UShelterBuilderComponent, SelectedShelterUnitIndex);
	DOREPLIFETIME(UShelterBuilderComponent, bIsBuilding);
}


bool UShelterBuilderComponent::BoxOverlapActors(UObject* WorldContextObject, const FVector BoxPos, const FQuat Rotation, FVector BoxExtent, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, UClass* ActorClassFilter, const TArray<AActor*>& ActorsToIgnore, TArray<AActor*>& OutActors)
{
	OutActors.Empty();

	TArray<UPrimitiveComponent*> OverlapComponents;
	bool bOverlapped = BoxOverlapComponents(WorldContextObject, BoxPos, Rotation, BoxExtent, ObjectTypes, NULL, ActorsToIgnore, OverlapComponents);
	if (bOverlapped)
	{
		UKismetSystemLibrary:: GetActorListFromComponentList(OverlapComponents, ActorClassFilter, OutActors);
	}

	return (OutActors.Num() > 0);
}

bool UShelterBuilderComponent::BoxOverlapComponents(UObject* WorldContextObject, const FVector BoxPos, const FQuat Rotation, FVector BoxExtent, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, UClass* ComponentClassFilter, const TArray<AActor*>& ActorsToIgnore, TArray<UPrimitiveComponent*>& OutComponents)
{
	OutComponents.Empty();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BoxOverlapComponents), false);
	Params.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;

	FCollisionObjectQueryParams ObjectParams;
	for (auto Iter = ObjectTypes.CreateConstIterator(); Iter; ++Iter)
	{
		const ECollisionChannel & Channel = UCollisionProfile::Get()->ConvertToCollisionChannel(false, *Iter);
		ObjectParams.AddObjectTypesToQuery(Channel);
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World != nullptr)
	{
		World->OverlapMultiByObjectType(Overlaps, BoxPos, Rotation, ObjectParams, FCollisionShape::MakeBox(BoxExtent), Params);
	}

	for (int32 OverlapIdx=0; OverlapIdx<Overlaps.Num(); ++OverlapIdx)
	{
		FOverlapResult const& O = Overlaps[OverlapIdx];
		if (O.Component.IsValid())
		{ 
			if ( !ComponentClassFilter || O.Component.Get()->IsA(ComponentClassFilter) )
			{
				OutComponents.Add(O.Component.Get());
			}
		}
	}

	return (OutComponents.Num() > 0);
}