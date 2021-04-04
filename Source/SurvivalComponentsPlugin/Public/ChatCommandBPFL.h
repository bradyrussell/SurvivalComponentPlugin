// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ChatCommandBase.h"
#include "ChatCommandBPFL.generated.h"


class AController;



/**
 * 
 */
UCLASS()
class SURVIVALCOMPONENTSPLUGIN_API UChatCommandBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,  meta = (WorldContext = "WorldContextObject"))
	static bool ExecuteChatCommand(UObject* WorldContextObject, TSubclassOf<UChatCommandBase> Command, const TArray<FString>& Params, AController* Instigator = nullptr);

	UFUNCTION(BlueprintCallable,  meta = (WorldContext = "WorldContextObject"))static FString GetParametersString(const FChatCommandDefinition& command) {
		FString result;
		for(auto& elem: command.RequiredParameterNames) {
			result += "<"+elem+"> ";
		}
		for(auto& elem: command.OptionalParameterNames) {
			result += "["+elem+"] ";
		}
		return result;
	}
	
	UFUNCTION(BlueprintCallable,  meta = (WorldContext = "WorldContextObject"))static FString GetHelpString(const FChatCommandDefinition& command) {
		return FString::Printf(TEXT("%s- %s"), *GetParametersString(command), *command.InfoString);
	}
};
