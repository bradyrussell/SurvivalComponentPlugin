// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay() {
	Super::BeginPlay();

	// ...

}

void UWeaponComponent::EquipTimerCompleted() {
	//UE_LOG(LogTemp, Warning, TEXT("Equipped weapon %s "), *PendingWeapon.WeaponName.ToString());
	const auto OldAmmo = CurrentWeapon.AmmoType;

	CurrentWeapon = PendingWeapon;
	canShoot = true;
	PendingWeapon = FWeaponProperties();

	if (!OldAmmo.IsNone() && LoadedAmmo > 0) { AmmoInventory->AddItem(FItemStack(OldAmmo, LoadedAmmo)); }

	// this will give reloads w the duration of the equip, not a problem as long as equip time isnt way shorter than relaod
	LoadedAmmo = 0;
	ReloadCompleted();

	OnEquip();
}

void UWeaponComponent::ShootingTimerCompleted() {
	canShoot = true;
	if (isShooting && shouldContinueShooting()) { Shoot(); }
}

void UWeaponComponent::AccuracyTimerCompleted() {
	// todo needs work. what criteria for regeneration? 
	if (!isShooting && --AccuracyShotCount > 0)
		GetWorld()->GetTimerManager().SetTimer(AccuracyTimer, this, &UWeaponComponent::AccuracyTimerCompleted, CurrentWeapon.RateOfFire, false);
}

void UWeaponComponent::ReloadCompleted() {
	//UE_LOG(LogTemp, Warning, TEXT("complete reload"));
	int AmountNeeded = CurrentWeapon.MagazineCapacity - LoadedAmmo;

	//UE_LOG(LogTemp, Warning, TEXT("need %d bullets"), AmountNeeded);

	auto Obtained = AmountNeeded - AmmoInventory->RemoveItem(FItemStack(CurrentWeapon.AmmoType, AmountNeeded)).Amount;

	//UE_LOG(LogTemp, Warning, TEXT("found %d bullets"), Obtained);

	LoadedAmmo += Obtained;

	isReloading = false;
	OnReload();
}

void UWeaponComponent::Shoot() {
	if (canShoot && !isReloading && isShooting) {
		if (LoadedAmmo > 0) {
			FWeaponAccuracy CurrentAccuracy = bIsAds ? CurrentWeapon.WeaponAccuracyADS : CurrentWeapon.WeaponAccuracyHipfire;
			
			for (int shots = 0; shots < CurrentWeapon.RoundsPerShot; shots++) {
				//todo remove temp debug
				AccuracyShotCount = TriggerShotCount;
				//todo remove when i implement accuracy shot count down

				//UE_LOG(LogTemp, Warning, TEXT("shoot %s"), CurrentWeapon.WeaponProjectile);
				const float pRecoil = FMath::RandRange(CurrentAccuracy.RecoilPitchRange.X, CurrentAccuracy.RecoilPitchRange.Y);
				const float yRecoil = FMath::RandRange(CurrentAccuracy.RecoilYawRange.X, CurrentAccuracy.RecoilYawRange.Y);
				const float PitchRecoil = (((float)AccuracyShotCount) * CurrentAccuracy.IncreaseRecoilPerShotFactor * pRecoil) + pRecoil;
				const float YawRecoil = (((float)AccuracyShotCount) * CurrentAccuracy.IncreaseRecoilPerShotFactor * yRecoil) + yRecoil;
				//todo separate TriggerShotCount and AccuracyShotCount, trigger zeroes on release, Recoil slowly ticks down per FireRate ticks
				const float pSpread = FMath::RandRange(CurrentAccuracy.SpreadPitchRange.X, CurrentAccuracy.SpreadPitchRange.Y);
				const float ySpread = FMath::RandRange(CurrentAccuracy.SpreadYawRange.X, CurrentAccuracy.SpreadYawRange.Y);
				const float PitchSpread = (((float)AccuracyShotCount) * CurrentAccuracy.IncreaseSpreadPerShotFactor * pSpread) + pSpread;
				const float YawSpread = (((float)AccuracyShotCount) * CurrentAccuracy.IncreaseSpreadPerShotFactor * ySpread) + ySpread;

				const FRotator AdjustedRotation = AimRotation.Add(PitchSpread, YawSpread, 0.f);

				if (CurrentWeapon.bIsRaycast) {
					FHitResult outHit;
					if (shots == 0) {
						LoadedAmmo--;
						TriggerShotCount++;
						AccuracyShotCount++;
						canShoot = false;
						GetWorld()->GetTimerManager().SetTimer(ShootingTimer, this, &UWeaponComponent::ShootingTimerCompleted, CurrentWeapon.RateOfFire, false);
						OnShoot();
						OnRecoil(FVector2D(PitchRecoil, YawRecoil));
					}
					FCollisionQueryParams qparam;
					qparam.AddIgnoredActor(GetOwner());
					qparam.bTraceComplex = true;

					auto b = GetWorld()->LineTraceSingleByChannel(outHit, AimVector, AimVector + (AdjustedRotation.Vector() * CurrentWeapon.RaycastLength), ECollisionChannel::ECC_Visibility,
					                                              qparam);
					if (b)
						OnRaycastHit(outHit);
				}
				else {
					FActorSpawnParameters params;
					params.Instigator = Cast<APawn>(this->GetOwner());
					params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					const auto Bullet = GetWorld()->SpawnActor(CurrentWeapon.WeaponProjectile.Get(), &AimVector, &AdjustedRotation, params);

					if (IsValid(Bullet) && shots == 0) {
						//UE_LOG(LogTemp, Warning, TEXT("happened %s, %s, %s"),*AimVector.ToCompactString(),* AimRotation.ToCompactString(), *Bullet->GetHumanReadableName());
						LoadedAmmo--;
						TriggerShotCount++;
						AccuracyShotCount++;
						canShoot = false;
						GetWorld()->GetTimerManager().SetTimer(ShootingTimer, this, &UWeaponComponent::ShootingTimerCompleted, CurrentWeapon.RateOfFire, false);
						OnShoot();
						OnRecoil(FVector2D(PitchRecoil, YawRecoil));
					}
				}
			}
		}
		else {
			//out of ammo click
			OnFailToShoot();
			StopShooting();
		}
	}
}

bool UWeaponComponent::shouldContinueShooting() const {
	//if(CurrentWeapon.FireMode == EFireMode::None || CurrentWeapon.FireMode == EFireMode::Semi) return false;
	if (CurrentWeapon.FireMode == EFireMode::Auto)
		return true;
	if (CurrentWeapon.FireMode == EFireMode::Burst)
		return TriggerShotCount < 3;
	return false;
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UWeaponComponent::StartShooting() {
	if (isShooting)
		return false;
	//UE_LOG(LogTemp, Warning, TEXT("start shooting"));
	isShooting = true;
	Shoot();
	//GetWorld()->GetTimerManager().SetTimer(AutoshootTimer, this, &UWeaponComponent::Shoot, CurrentWeapon.RateOfFire, true);
	return true;
}

bool UWeaponComponent::StopShooting() {
	isShooting = false;
	TriggerShotCount = 0;
	return true;
}

void UWeaponComponent::StartAimDownSight() {
	bIsAds = true;
	OnAimDownSightStart();
}

void UWeaponComponent::StopAimDownSight() {
	bIsAds = false;
	OnAimDownSightEnd();
}

void UWeaponComponent::Aim(FVector InVector, FRotator InRot) {
	AimVector = InVector;
	AimRotation = InRot;
}

bool UWeaponComponent::Reload() {
	if (canShoot && !isReloading && LoadedAmmo < CurrentWeapon.MagazineCapacity) {
		if (AmmoInventory->hasItem(FItemStack(CurrentWeapon.AmmoType, 1))) {
			//UE_LOG(LogTemp, Warning, TEXT("start reload"));
			isReloading = true;
			GetWorld()->GetTimerManager().SetTimer(ReloadingTimer, this, &UWeaponComponent::ReloadCompleted, CurrentWeapon.ReloadDuration, false);
			return true;
		}
	}
	return false;
}

bool UWeaponComponent::SetWeapon(FWeaponProperties NewWeapon) {
	//UE_LOG(LogTemp, Warning, TEXT("start equip"));
	PendingWeapon = NewWeapon;
	GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UWeaponComponent::EquipTimerCompleted, PendingWeapon.EquipDuration, false);
	return true;
}

void UWeaponComponent::SetWeaponInventory(UBaseInventoryComponent* NewInventory) { AmmoInventory = NewInventory; }

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	DOREPLIFETIME(UWeaponComponent, LoadedAmmo);
	DOREPLIFETIME(UWeaponComponent, bIsAds);
	DOREPLIFETIME(UWeaponComponent, isShooting);
	DOREPLIFETIME(UWeaponComponent, canShoot);
	DOREPLIFETIME(UWeaponComponent, isReloading);
	DOREPLIFETIME(UWeaponComponent, AimVector);
	DOREPLIFETIME(UWeaponComponent, CurrentWeapon);
}
