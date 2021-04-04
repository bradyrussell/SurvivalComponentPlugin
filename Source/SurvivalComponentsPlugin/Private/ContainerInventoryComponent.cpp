// Fill out your copyright notice in the Description page of Project Settings.


#include "ContainerInventoryComponent.h"
#include "Net/UnrealNetwork.h"

bool UContainerInventoryComponent::CanPlayerInteract_Implementation(APlayerController* Player) {
	if(Player && Player->GetPawn()) {
		return !bIsLocked && FVector::DistSquared(GetOwner()->GetActorLocation(), Player->GetPawn()->GetActorLocation()) <= InteractDistanceSquared;
	}
	return false;
}

void UContainerInventoryComponent::setLocked(bool Locked) {
	bIsLocked = Locked;
}

void UContainerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UContainerInventoryComponent, bIsLocked);
	DOREPLIFETIME(UContainerInventoryComponent, ContainerTitle);
	DOREPLIFETIME(UContainerInventoryComponent, ContainerOwner);
}