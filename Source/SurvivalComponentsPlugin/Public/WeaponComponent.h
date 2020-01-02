// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponStructs.h"
#include "BaseInventoryComponent.h"
#include "WeaponComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALCOMPONENTSPLUGIN_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 LoadedAmmo;
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated) int32 UnloadedAmmo;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) int32 TriggerShotCount;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) int32 AccuracyShotCount;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) bool isShooting;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) bool canShoot;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) bool isReloading;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) FVector AimVector;
	FRotator AimRotation;
	
	FTimerHandle AccuracyTimer;
	FTimerHandle ShootingTimer;
	FTimerHandle ReloadingTimer;
	FTimerHandle EquipTimer;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) FWeaponProperties CurrentWeapon;
	FWeaponProperties PendingWeapon;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere) UBaseInventoryComponent* AmmoInventory;

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void EquipTimerCompleted();
	void ShootingTimerCompleted();
	void AccuracyTimerCompleted();
	void ReloadCompleted();
	void Shoot();
	bool shouldContinueShooting() const;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated) bool bIsAds = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool StartShooting();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool StopShooting();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void StartAimDownSight();
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void StopAimDownSight();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void Aim(FVector InVector, FRotator InRot);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool Reload();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) bool SetWeapon(FWeaponProperties NewWeapon);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly) void SetWeaponInventory(UBaseInventoryComponent* NewInventory);

	UFUNCTION(BlueprintImplementableEvent) void OnShoot();
	UFUNCTION(BlueprintImplementableEvent) void OnReload();
	UFUNCTION(BlueprintImplementableEvent) void OnEquip();
	UFUNCTION(BlueprintImplementableEvent) void OnFailToShoot();
	UFUNCTION(BlueprintImplementableEvent) void OnRaycastHit(FHitResult HitResult);
	UFUNCTION(BlueprintImplementableEvent) void OnRecoil(FVector2D PitchYaw);
	UFUNCTION(BlueprintImplementableEvent) void OnAimDownSightStart();
	UFUNCTION(BlueprintImplementableEvent) void OnAimDownSightEnd();
};
