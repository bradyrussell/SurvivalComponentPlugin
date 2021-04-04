/*// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingUnitBase.h"
#include "DatabaseProvider.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Misc/FileHelper.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"


// Sets default values
ABuildingUnitBase::ABuildingUnitBase() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(BuildingMesh);

	BU_Type = FName(*this->GetName());
}

// Called when the game starts or when spawned
void ABuildingUnitBase::BeginPlay() {
	Super::BeginPlay();
	socketedChildren.AddDefaulted(BuildingMesh->GetAllSocketNames().Num());
}

// Called every frame
void ABuildingUnitBase::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

FName ABuildingUnitBase::getNearestSocket(FVector location) const {
	float distance = INFINITY;
	FName closest = NAME_None;

	auto sockets = BuildingMesh->GetAllSocketNames();
	for (auto& socket : sockets) {
		const float tempDist = FVector::DistSquared(location, BuildingMesh->GetSocketLocation(socket));
		if (distance > tempDist) {
			closest = socket;
			distance = tempDist;
		}
	}

	return closest;
}

FName ABuildingUnitBase::getNearestSocketOfType(FVector location, FString type) const {
	float distance = INFINITY;
	FName closest = NAME_None;

	auto sockets = BuildingMesh->GetAllSocketNames();
	for (auto& socket : sockets) {
		float tempDist = FVector::DistSquared(location, BuildingMesh->GetSocketLocation(socket));
		auto parsed = parseSocketName(socket);
		if (parsed.type.Equals(type, ESearchCase::IgnoreCase) && distance > tempDist) {
			closest = socket;
			distance = tempDist;
		}
	}

	return closest;
}

FBuildingSocket ABuildingUnitBase::parseSocketName(FName socket) {
	FString socket_name = socket.ToString();

	FBuildingSocket ret;
	socket_name.Split("_", &ret.axis, &ret.type);

	return ret;
}

void ABuildingUnitBase::AddSocketedChild(const FName socket, ABuildingUnitBase* child) {
	// manage attachment
	const auto socketIndex = getSocketIndex(socket);
	if (!socketedChildren.IsValidIndex(socketIndex)) {
		UE_LOG(LogTemp, Warning, TEXT("Tried to attach BU to nonexistent socket %s, index: %d."), *socket.ToString(), socketIndex);
		return;
	}

	if(IsValid(socketedChildren[socketIndex])) {
		UE_LOG(LogTemp, Warning, TEXT("Tried to attach BU to filled socket %s, index: %d."), *socket.ToString(), socketIndex);
		return;
	}
	
	child->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, socket);
	UE_LOG(LogTemp, Warning, TEXT("Attached to socket index: %d, num: %d"), getSocketIndex(socket), socketedChildren.Num());
	socketedChildren[socketIndex] = child;
}

void ABuildingUnitBase::AddSocketedChild(int32 socket_index, ABuildingUnitBase* child) {
	const FName socket = BuildingMesh->GetAllSocketNames()[socket_index];
	
	if (!socketedChildren.IsValidIndex(socket_index)) {
		UE_LOG(LogTemp, Warning, TEXT("Tried to attach BU to nonexistent socket %s, index: %d."), *socket.ToString(), socket_index);
		return;
	}

	if(IsValid(socketedChildren[socket_index])) {
		UE_LOG(LogTemp, Warning, TEXT("Tried to attach BU to filled socket %s, index: %d."), *socket.ToString(), socket_index);
		return;
	}
	
	child->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, socket);
	UE_LOG(LogTemp, Warning, TEXT("Attached to socket index: %d, num: %d"), getSocketIndex(socket), socketedChildren.Num());
	socketedChildren[socket_index] = child;
}

void ABuildingUnitBase::AddUnsocketedChild(const FTransform attachment, ABuildingUnitBase* child) {
	child->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	FUnsocketedAttachment attach;
	attach.transform = attachment;
	attach.child = child;
	attach.parent = this;
	unsocketedChildren.Emplace(attach);
}

int32 ABuildingUnitBase::getSocketIndex(FName socket) const {
	const auto sockets = BuildingMesh->GetAllSocketNames();
	return sockets.Find(socket);
}

std::string ABuildingUnitBase::RecursiveSerialize(ABuildingUnitBase* RootBuildingUnitBase) {
	FBufferArchive buf;

	auto SerializedBuildingUnit = RootBuildingUnitBase->SerializeToStruct();
	
	buf << SerializedBuildingUnit;

	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize has finished, %d bytes."),buf.Num());
	
	return std::string((char*)buf.GetData(), buf.Num());
}

FString ABuildingUnitBase::NewSerializeTest(ABuildingUnitBase* RootBuildingUnitBase) {
	FBufferArchive buf(true);
	auto SerializedBuildingUnit = RootBuildingUnitBase->RecursiveGetRoot()->SerializeToStruct();
	
	buf << SerializedBuildingUnit;

	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize has finished, %d bytes."),buf.Num());

	FString ffs;

	for(auto &elem:buf) {
		if(elem > 32 && elem < 126) ffs.AppendChar(elem);
		else {
			ffs.Append(" ");
			ffs.AppendInt(elem);
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize produced the following archive: %s"), *ffs);

	auto a = std::string((char*)buf.GetData(), buf.Num());
	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize produced a string of length: %d"), a.length());
	
	
	//FSerializedBuildingUnit verified;
	//ver << verified;
	TArray<uint8> compressedData;
	FArchiveSaveCompressedProxy compress = FArchiveSaveCompressedProxy(compressedData, NAME_Zlib);

	compress << buf;
	compress.Flush();
	
	FFileHelper::SaveArrayToFile(compressedData, TEXT("C:/Users/Admin/Documents/Unreal Projects/BuildSystem/BaseStructure.zcb"));
	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize compressed archive length: %d"), compressedData.Num());

	compress.Close();

	// FSerializedBuilding sb;
	// sb.Root = SerializedBuildingUnit;
	// sb.RootTransform = RootBuildingUnitBase->RecursiveGetRoot()->GetActorTransform();
	// sb.RootTransform.AddToTranslation(FVector(0,0,10000));
	//
	// DeserializeBuildingFromStruct(RootBuildingUnitBase,sb);
	
	return ffs;
}

// bool ABuildingUnitBase::SaveToFile(ABuildingUnitBase* RootBuildingUnitBase) {
// 	FBufferArchive buf(true);
//
// 	const auto Root = RootBuildingUnitBase->RecursiveGetRoot();
// 	auto SerializedBuildingUnit = Root->SerializeToStruct();
//
// 	FSerializedBuilding sb;
// 	sb.Root = SerializedBuildingUnit;
// 	sb.RootTransform = Root->GetActorTransform();
// 	
// 	//////////////////
// 	buf << sb;
// 	
// 	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize has finished, %d bytes."),buf.Num());
//
// 	TArray<uint8> compressedData;
// 	FArchiveSaveCompressedProxy compress = FArchiveSaveCompressedProxy(compressedData, NAME_Zlib);
//
// 	compress << buf;
// 	compress.Flush();
// 	
// 	FFileHelper::SaveArrayToFile(compressedData, BASEFILE);
// 	UE_LOG(LogTemp, Warning, TEXT("RecursiveSerialize compressed archive length: %d"), compressedData.Num());
//
// 	compress.Close();
// 	return true;
// }
//
// bool ABuildingUnitBase::LoadFromFile(UObject* WorldContextObject,FTransform RootLocation) {
// 	TArray<uint8> data, rawData;
// 	FFileHelper::LoadFileToArray(data,BASEFILE);
//
// 	FArchiveLoadCompressedProxy decompress(data, NAME_Zlib);
//
// 	decompress << rawData;
// 	
// 	FMemoryReader reader(rawData, true);
// 	
// 	FSerializedBuilding sb;
//
// 	reader << sb;
//
// 	sb.RootTransform = RootLocation;
// 	
// 	DeserializeBuildingFromStruct(WorldContextObject,sb);
// 	
// 	return true;
// }

// override this function to write metadata if need be
FSerializedBuildingUnit ABuildingUnitBase::SerializeToStruct() {
	FSerializedBuildingUnit sbu;
	const auto Count = socketedChildren.Num();

	sbu.BU_Index = UDatabaseProvider::BuildingUnitToIndex(this, this->GetClass());
	sbu.SocketedAttachments.AddDefaulted(Count);

	for (int i = 0; i < Count; ++i) {
		if(IsValid(socketedChildren[i])) {
			//
			sbu.SocketedAttachments[i] = socketedChildren[i]->SerializeToStruct();
		}
	}

	for(auto &elem:unsocketedChildren) {
		sbu.UnsocketedAttachmentsTransforms.Add(elem.transform);
		sbu.UnsocketedAttachments.Add(elem.child->SerializeToStruct());
	}

	//dummy metadata
	sbu.Metadata.Add(FMath::RandRange(0,255));
	sbu.Metadata.Add(FMath::RandRange(0,255));
	sbu.Metadata.Add(FMath::RandRange(0,255));
	sbu.Metadata.Add(FMath::RandRange(0,255));
	// end dummy meta
	
	return sbu;
}

ABuildingUnitBase* ABuildingUnitBase::DeserializeFromStruct(UObject* WorldContextObject, FSerializedBuildingUnit SerializedBuildingUnit, FTransform transform) {
	const auto buClass = UDatabaseProvider::IndexToBuildingUnit(WorldContextObject, SerializedBuildingUnit.BU_Index);

	UE_LOG(LogTemp, Warning, TEXT("DeserializeFromStruct: Deserializing a %s."), *buClass->GetName());
	
	auto newBU = WorldContextObject->GetWorld()->SpawnActorDeferred<ABuildingUnitBase>(buClass, transform,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	newBU->FinishSpawning(transform);//todo
	
	for (int i = 0; i < SerializedBuildingUnit.SocketedAttachments.Num(); i++) { // 
		if(SerializedBuildingUnit.SocketedAttachments[i].BU_Index != -1) {
		//const auto transform2 = newBU->BuildingMesh->GetSocketTransform(newBU->BuildingMesh->GetAllSocketNames()[i]); // not needed
		newBU->AddSocketedChild(i,DeserializeFromStruct(WorldContextObject,SerializedBuildingUnit.SocketedAttachments[i]/*, transform2#1#));
		}
	}

	for(int i = 0; i < SerializedBuildingUnit.UnsocketedAttachmentsTransforms.Num(); i++) {
		newBU->AddUnsocketedChild(SerializedBuildingUnit.UnsocketedAttachmentsTransforms[i],DeserializeFromStruct(WorldContextObject,SerializedBuildingUnit.UnsocketedAttachments[i]));
	}

	FString meta;
	for(auto &elem:SerializedBuildingUnit.Metadata) {
		meta.AppendInt(elem);
		meta.Append(" ");
	}

	UE_LOG(LogTemp, Warning, TEXT("DeserializeFromStruct: Deserialized metadata %s."), *meta);
	
	return newBU;
}

ABuildingUnitBase* ABuildingUnitBase::DeserializeBuildingFromStruct(UObject* WorldContextObject, FSerializedBuilding SerializedBuilding) {
	return DeserializeFromStruct(WorldContextObject, SerializedBuilding.Root, SerializedBuilding.RootTransform);
}


ABuildingUnitBase* ABuildingUnitBase::RecursiveGetRoot() {
	ABuildingUnitBase* parent = Cast<ABuildingUnitBase>(GetAttachParentActor());

	if (parent && IsValid(parent)) {
		UE_LOG(LogTemp, Warning, TEXT("RecursiveFindParent: Stepping up to %s."), *parent->GetName());
		return parent->RecursiveGetRoot();
	}
	UE_LOG(LogTemp, Warning, TEXT("RecursiveFindParent: Found parent %s."), *this->GetName());
	check(UDatabaseProvider::BuildingUnitToIndex(this, this->GetClass()) == 3);
	return this;
}
/*
 *
 * 	for(auto &elem:BuildingUnit.socketedChildren) {
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
 #1#*/