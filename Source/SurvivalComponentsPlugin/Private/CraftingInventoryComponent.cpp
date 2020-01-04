// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingInventoryComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"


bool UCraftingInventoryComponent::QueueRecipe(FProcessingRecipe InRecipe) {
	if (InRecipe.RecipeType == Type) {
		ProcessingQueue.Enqueue(InRecipe);
		Multi_QueuedProcessingRecipe(InRecipe);
	}
	return InRecipe.RecipeType == Type;
}

bool UCraftingInventoryComponent::BeginProcessing() {
	if (bIsCurrentlyProcessing || ProcessingQueue.IsEmpty())
		return false;

	setLocked(true);
	ProcessingQueue.Dequeue(CurrentJob_Recipe);

	if (CurrentJob_Recipe.ProcessingDuration == 0.f) {
		// zero duration , complete instantly
		EndProcessing();
		Multi_BeginProcessingRecipe(CurrentJob_Recipe);
		return true;
	}

	GetWorld()->GetTimerManager().SetTimer(CurrentJob_Timer, this, &UCraftingInventoryComponent::EndProcessing, CurrentJob_Recipe.ProcessingDuration, false);
	if (CurrentJob_Timer.IsValid())
		bIsCurrentlyProcessing = true;
	//Client_ExpectedFinish = FDateTime::Now()
	Multi_BeginProcessingRecipe(CurrentJob_Recipe);
	return bIsCurrentlyProcessing;
}

FProcessingRecipe UCraftingInventoryComponent::PeekNextRecipe() const {
	FProcessingRecipe out;
	ProcessingQueue.Peek(out);
	return out;
}

bool UCraftingInventoryComponent::AddFuel(float Amount) {
	if (Fuel >= MaximumFuel)
		return false;
	Fuel = FMath::Min(MaximumFuel, Fuel + Amount);
	return true;
}

void UCraftingInventoryComponent::Multi_BeginProcessingRecipe_Implementation(FProcessingRecipe Recipe) {
	if (BeginProcessingRecipe_Event.IsBound())
		BeginProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::Multi_FinishProcessingRecipe_Implementation(FProcessingRecipe Recipe) {
	if (FinishProcessingRecipe_Event.IsBound())
		FinishProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::Multi_FailedProcessingRecipe_Implementation(FProcessingRecipe Recipe) {
	if (FailedProcessingRecipe_Event.IsBound())
		FailedProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::Multi_QueuedProcessingRecipe_Implementation(FProcessingRecipe Recipe) {
	if (QueuedProcessingRecipe_Event.IsBound())
		QueuedProcessingRecipe_Event.Broadcast(Recipe);
}

void UCraftingInventoryComponent::EndProcessing() {
	if (!bIsCurrentlyProcessing)
		return;

	// do crafting recipe
	if (hasItems(CurrentJob_Recipe.InputItems) &&
		(!CurrentJob_Recipe.bRequiresCatalyst || hasItems(CurrentJob_Recipe.CatalystItems)) &&
		(!CurrentJob_Recipe.bRequiresFuel || CurrentJob_Recipe.FuelRequired <= Fuel)) {
		if (CurrentJob_Recipe.bRequiresFuel)
			Fuel -= CurrentJob_Recipe.FuelRequired;

		if (areAllEmpty(RemoveItems(CurrentJob_Recipe.InputItems))) {
			auto OutItems = CurrentJob_Recipe.OutputItems;
			if (CurrentJob_Recipe.bHasVariableYield) {
				for (auto& elem : OutItems) {
					float yield = FMath::RandRange(CurrentJob_Recipe.VariableYieldRange.X, CurrentJob_Recipe.VariableYieldRange.Y);
					elem.Amount *= yield;
				}
			}

			auto overflow = AddItems(OutItems);
			if(!areAllEmpty(overflow)) { // no room todo maybe drop in world?
				CancelAndClearQueue();
				return;
			}

		}
		Multi_FinishProcessingRecipe(CurrentJob_Recipe);
	}
	else {
		if (CurrentJob_Recipe.FuelRequired > Fuel)
			ClearQueue();

		Multi_FailedProcessingRecipe(CurrentJob_Recipe);
	}

	CurrentJob_Timer.Invalidate();
	bIsCurrentlyProcessing = false;
	setLocked(false);

	if (!ProcessingQueue.IsEmpty())
		BeginProcessing();
}

void UCraftingInventoryComponent::CancelProcessing() {
	if (!bIsCurrentlyProcessing)
		return;
	bIsCurrentlyProcessing = false;
	CurrentJob_Timer.Invalidate();
	setLocked(false);
	Multi_FailedProcessingRecipe(CurrentJob_Recipe);
}

void UCraftingInventoryComponent::ClearQueue() { ProcessingQueue.Empty(); }

void UCraftingInventoryComponent::CancelAndClearQueue() {
	ClearQueue();
	CancelProcessing();
}

bool UCraftingInventoryComponent::isCurrentlyProcessing() const { return bIsCurrentlyProcessing; }

float UCraftingInventoryComponent::GetRemainingTime() const { return GetWorld()->GetTimerManager().GetTimerRemaining(CurrentJob_Timer); }

float UCraftingInventoryComponent::GetElapsedTime() const { return GetWorld()->GetTimerManager().GetTimerElapsed(CurrentJob_Timer); }

void UCraftingInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(UContainerInventoryComponent, bIsLocked);
	DOREPLIFETIME(UBaseInventoryComponent, NumberSlots);
	DOREPLIFETIME(UBaseInventoryComponent, InventorySlots);
	DOREPLIFETIME(UCraftingInventoryComponent, Type);
	DOREPLIFETIME(UCraftingInventoryComponent, Fuel);
	DOREPLIFETIME(UCraftingInventoryComponent, bIsCurrentlyProcessing);
	DOREPLIFETIME(UCraftingInventoryComponent, CurrentJob_Recipe);
	//DOREPLIFETIME(UCraftingInventoryComponent, CurrentJob_Timer);
	//DOREPLIFETIME(UCraftingInventoryComponent, Client_ExpectedFinish);
}
