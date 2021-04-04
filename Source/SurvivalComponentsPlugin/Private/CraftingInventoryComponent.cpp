// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingInventoryComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "IDatabaseProvider.h"
#include "Kismet/GameplayStatics.h"


UDataTable* UCraftingInventoryComponent::GetRecipesDataTable() const {
	const auto DB = IIDatabaseProvider::Execute_GetRecipeDefinitions(UGameplayStatics::GetGameInstance(this));
	check(DB);
	return DB;	
}

FProcessingRecipe* UCraftingInventoryComponent::GetRecipeDefinition(FName Recipe) const{
	return GetRecipesDataTable()->FindRow<FProcessingRecipe>(Recipe, TEXT("UCraftingInventoryComponent::GetRecipeDefinition(FName Recipe)"));
}

bool UCraftingInventoryComponent::QueueRecipe(FName InRecipe) {
	UE_LOG(LogTemp,Warning,TEXT("Queue Recipe %s"), *InRecipe.ToString());
	const auto RecipeDefinition = GetRecipeDefinition(InRecipe);
	if(!RecipeDefinition) return false;
	if (RecipeDefinition->RecipeType == Type) {
		ProcessingQueue.Add(InRecipe);
		Multi_QueuedProcessingRecipe(InRecipe);
	}
	return RecipeDefinition->RecipeType == Type;
}

bool UCraftingInventoryComponent::BeginProcessing() {
	UE_LOG(LogTemp,Warning,TEXT("Pre Begin Processing %d"), ProcessingQueue.Num());
	if (bIsCurrentlyProcessing || ProcessingQueue.Num() == 0)
		return false;

	//setLocked(true);
	CurrentJob = ProcessingQueue[0];
	ProcessingQueue.RemoveAt(0);

	UE_LOG(LogTemp,Warning,TEXT("Begin Processing %s"), *CurrentJob.ToString());
	
	const auto RecipeDefinition = GetRecipeDefinition(CurrentJob);
	if (RecipeDefinition->ProcessingDuration <= 0.f) {
		// zero duration , complete instantly
		UE_LOG(LogTemp,Warning,TEXT("Instant Processing %s"), *CurrentJob.ToString());
		Multi_BeginProcessingRecipe(CurrentJob);
		EndProcessing();
		return true;
	}

	GetWorld()->GetTimerManager().SetTimer(CurrentJob_Timer, this, &UCraftingInventoryComponent::EndProcessing, RecipeDefinition->ProcessingDuration, false);
	if (CurrentJob_Timer.IsValid())
		bIsCurrentlyProcessing = true;
	//Client_ExpectedFinish = FDateTime::Now()
	Multi_BeginProcessingRecipe(CurrentJob);
	return bIsCurrentlyProcessing;
}

FName UCraftingInventoryComponent::PeekNextRecipe() const {
	return ProcessingQueue[0];
}

bool UCraftingInventoryComponent::AddFuel(float Amount) {
	if (Fuel >= MaximumFuel)
		return false;
	Fuel = FMath::Min(MaximumFuel, Fuel + Amount);
	return true;
}

TArray<FName> UCraftingInventoryComponent::GetAvailableRecipes() {
	UDataTable* RecipesTable = GetRecipesDataTable();

	TArray<FName> AvailableRecipes;
	
	for(auto& Recipe:RecipesTable->GetRowNames()) {
		FProcessingRecipe* ProcessingRecipe = GetRecipeDefinition(Recipe);
		if (HasItems(ProcessingRecipe->InputItems) &&
		(!ProcessingRecipe->bRequiresCatalyst || HasItems(ProcessingRecipe->CatalystItems)) &&
		(!ProcessingRecipe->bRequiresFuel || ProcessingRecipe->FuelRequired <= Fuel)) {
			AvailableRecipes.Add(Recipe);
		}
	}

	return AvailableRecipes;
}

void UCraftingInventoryComponent::Multi_BeginProcessingRecipe_Implementation(FName Recipe) {
	if (BeginProcessingRecipe_Event.IsBound())
		BeginProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::Multi_FinishProcessingRecipe_Implementation(FName Recipe) {
	if (FinishProcessingRecipe_Event.IsBound())
		FinishProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::Multi_FailedProcessingRecipe_Implementation(FName Recipe) {
	if (FailedProcessingRecipe_Event.IsBound())
		FailedProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::Multi_QueuedProcessingRecipe_Implementation(FName Recipe) {
	if (QueuedProcessingRecipe_Event.IsBound())
		QueuedProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::EndProcessing() {
	if (!bIsCurrentlyProcessing)
		return;

	UE_LOG(LogTemp,Warning,TEXT("End Processing %s"), *CurrentJob.ToString());
	
	const auto RecipeDefinition = GetRecipeDefinition(CurrentJob);
	
	// do crafting recipe
	if (HasItems(RecipeDefinition->InputItems) &&
		(!RecipeDefinition->bRequiresCatalyst || HasItems(RecipeDefinition->CatalystItems)) &&
		(!RecipeDefinition->bRequiresFuel || RecipeDefinition->FuelRequired <= Fuel)) {
		if (RecipeDefinition->bRequiresFuel)
			Fuel -= RecipeDefinition->FuelRequired;

		if (AreAllEmpty(RemoveItems(RecipeDefinition->InputItems))) {
			auto OutItems = RecipeDefinition->OutputItems;
			if (RecipeDefinition->bHasVariableYield) {
				for (auto& elem : OutItems) {
					const float yield = FMath::RandRange(RecipeDefinition->VariableYieldRange.X, RecipeDefinition->VariableYieldRange.Y);
					elem.Amount *= yield;
				}
			}

			const auto overflow = AddItems(OutItems);
			if(!AreAllEmpty(overflow)) { // no room todo maybe drop in world?
				CancelAndClearQueue();
				return;
			}

		}
		Multi_FinishProcessingRecipe(CurrentJob);
	}
	else {
		/*if (RecipeDefinition->FuelRequired > Fuel)
			ClearQueue();*/
		ProcessingQueue.Remove(CurrentJob); // remove any further copies of the same recipe in the queue
		Multi_FailedProcessingRecipe(CurrentJob);
	}

	GetWorld()->GetTimerManager().ClearTimer(CurrentJob_Timer);
	bIsCurrentlyProcessing = false;
	//setLocked(false);

	if (ProcessingQueue.Num() > 0)
		BeginProcessing();
}

void UCraftingInventoryComponent::CancelProcessing() {
	if (!bIsCurrentlyProcessing)
		return;
	bIsCurrentlyProcessing = false;
	GetWorld()->GetTimerManager().ClearTimer(CurrentJob_Timer);
	//setLocked(false);
	Multi_FailedProcessingRecipe(CurrentJob);
}

void UCraftingInventoryComponent::ClearQueue() { ProcessingQueue.Empty(); }

void UCraftingInventoryComponent::CancelAndClearQueue() {
	ClearQueue();
	CancelProcessing();
}

bool UCraftingInventoryComponent::IsCurrentlyProcessing() const { return bIsCurrentlyProcessing; }

float UCraftingInventoryComponent::GetRemainingTime() const { return GetWorld()->GetTimerManager().GetTimerRemaining(CurrentJob_Timer); }

float UCraftingInventoryComponent::GetElapsedTime() const { return GetWorld()->GetTimerManager().GetTimerElapsed(CurrentJob_Timer); }

void UCraftingInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCraftingInventoryComponent, Type);
	DOREPLIFETIME(UCraftingInventoryComponent, Fuel);
	DOREPLIFETIME(UCraftingInventoryComponent, bIsCurrentlyProcessing);
	DOREPLIFETIME(UCraftingInventoryComponent, CurrentJob);
	DOREPLIFETIME(UCraftingInventoryComponent, ProcessingQueue);
}
