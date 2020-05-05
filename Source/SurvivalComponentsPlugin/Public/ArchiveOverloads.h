#pragma once
#include "BuildingUnitBase.h"

// this currently traverses a tree in post order
/*FORCEINLINE FArchive& operator<<(FArchive& Ar, ABuildingUnitBase& BuildingUnit) {
	if (!IsValid(&BuildingUnit))
		return Ar;

	FName BU_Type = "BuildingUnitBase";
	FTransform Transform = BuildingUnit.GetActorTransform();

	Ar << BU_Type;
	//Ar << Transform; //we only want the root's transform or unsocketed attachments

	// look up this meshes number of sockets 

	for(auto &elem:BuildingUnit.socketedChildren) {
		//   save each socket's child or null
		Ar << *elem;
	}

	// save number of unsocketed attachments
	int32 usChildren = BuildingUnit.unsocketedChildren.Num();
	Ar << usChildren;

	for(auto &elem:BuildingUnit.unsocketedChildren) {
		// save each unsocketed attachment and its transform
		Ar << elem.transform;
		Ar << *elem.child;
	}

	return Ar;
}*/

/*// this currently traverses a tree in post order
FORCEINLINE FArchive& operator<<(FArchive& Ar, FSerializedBuildingUnit &SerializedBuildingUnit) {

	const FString saveLoad = Ar.IsSaving() ? "Saving" : "Loading";

	Ar << SerializedBuildingUnit.BU_Index;

	UE_LOG(LogTemp, Warning, TEXT("%s: Type %d"), *saveLoad, SerializedBuildingUnit.BU_Index);
	
	if(SerializedBuildingUnit.BU_Index < 0) return Ar; // dont write null nodes children. just saves space 
	
	UE_LOG(LogTemp, Warning, TEXT("-- %s: Begin %s attachments. "),*saveLoad, *saveLoad,SerializedBuildingUnit.SocketedAttachments.Num());
	
	Ar << SerializedBuildingUnit.SocketedAttachments;

	UE_LOG(LogTemp, Warning, TEXT("-- %s: %d SocketedAttachments"),*saveLoad, SerializedBuildingUnit.SocketedAttachments.Num());

	Ar << SerializedBuildingUnit.UnsocketedAttachmentsTransforms;
	Ar << SerializedBuildingUnit.UnsocketedAttachments;

	UE_LOG(LogTemp, Warning, TEXT("-- %s: %d UnsocketedAttachments, and %d Transforms"),*saveLoad, SerializedBuildingUnit.UnsocketedAttachments.Num(),SerializedBuildingUnit.UnsocketedAttachmentsTransforms.Num());
	check(SerializedBuildingUnit.UnsocketedAttachments.Num() == SerializedBuildingUnit.UnsocketedAttachmentsTransforms.Num());

	Ar << SerializedBuildingUnit.Metadata;

	UE_LOG(LogTemp, Warning, TEXT("-- %s: %d b metadata"),*saveLoad, SerializedBuildingUnit.Metadata.Num());
	
	return Ar;
}

FORCEINLINE FArchive& operator<<(FArchive& Ar, FSerializedBuilding &SerializedBuilding) {

	const FString saveLoad = Ar.IsSaving() ? "Saving" : "Loading";

	Ar << SerializedBuilding.RootTransform;
	
	UE_LOG(LogTemp, Warning, TEXT("= %s building with root at %s."), *saveLoad, *SerializedBuilding.RootTransform.ToHumanReadableString());
	
	Ar << SerializedBuilding.Root;

	Ar << SerializedBuilding.Metadata;

	UE_LOG(LogTemp, Warning, TEXT("= %s: %d b metadata"),*saveLoad, SerializedBuilding.Metadata.Num());
	
	return Ar;
}*/