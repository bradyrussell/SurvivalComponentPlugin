// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatCommandBPFL.h"
#include "Engine/World.h"

bool UChatCommandBPFL::ExecuteChatCommand(UObject* WorldContextObject, TSubclassOf<UChatCommandBase> Command, const TArray<FString>& Params, AController* Instigator) {
	auto CommandObject = NewObject<UChatCommandBase>(WorldContextObject, Command);
	
	const auto retval = CommandObject->OnCommandExecuted(Params, Instigator, WorldContextObject->GetWorld()->GetAuthGameMode());

	CommandObject->ConditionalBeginDestroy(); // we immediately destroy the effect object
	CommandObject = nullptr;
	
	return retval;
}
