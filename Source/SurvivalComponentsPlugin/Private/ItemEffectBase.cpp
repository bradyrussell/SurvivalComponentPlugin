// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemEffectBase.h"


UItemEffectBase::~UItemEffectBase() {
	//Super::Dest
	UE_LOG(LogTemp, Warning, TEXT("ItemEffect %s has been destroyed."), *GetClass()->GetName());
}
