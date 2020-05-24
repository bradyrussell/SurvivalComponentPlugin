// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameModeBase.h"
#include "ChatCommandBase.generated.h"

class AController;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SURVIVALCOMPONENTSPLUGIN_API UChatCommandBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent) bool OnCommandExecuted(const TArray<FString>& Params, AController* Instigator = nullptr, AGameModeBase* GameMode = nullptr);
};

USTRUCT(BlueprintType)
	struct FChatCommandDefinition : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Chat Command") FString Permission;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Chat Command") TSubclassOf<UChatCommandBase> Command;

};