// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "ContainerInventoryComponent.generated.h"



/**
 * 
 */
UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), Blueprintable )
class SURVIVALCOMPONENTSPLUGIN_API UContainerInventoryComponent : public UBaseInventoryComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Inventory - Container", SaveGame) FText ContainerTitle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Inventory - Container", SaveGame) FString ContainerOwner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Inventory - Container", SaveGame) bool bIsLocked = false;

	// controllers who are currently looking at inventory contents
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Inventory - Container") TArray<APlayerController*> CurrentViewers;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory - Container") void setLocked(bool Locked);

	UFUNCTION(BlueprintPure, Category="Inventory - Container") bool IsLocked() const { return bIsLocked; }
};
