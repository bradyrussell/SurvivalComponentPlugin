#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "BulletBase.h"
#include "Engine/SkeletalMesh.h"
#include "Sound/SoundCue.h"
#include "MobEntityBase.h"
#include "WeaponStructs.generated.h"

//  USTRUCT( BlueprintType )
// struct FTestStruct : public FTableRowBase
// {
//      GENERATED_USTRUCT_BODY()
//  
//      UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestStruct")
//      int32 test;
//  };
//
//

UENUM(BlueprintType)
enum class EFireMode : uint8 {
	None UMETA(DisplayName="None"),
	Semi UMETA(DisplayName="Semi-automatic"),
	Burst UMETA(DisplayName="Burst"),
	Auto UMETA(DisplayName="Automatic"),
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8 {
	//None 	UMETA(DisplayName="None"),
	Weapon UMETA(DisplayName="Weapon"),
	WeaponScope UMETA(DisplayName="Weapon Scope"),
	WeaponGrip UMETA(DisplayName="Weapon Grip"),
	WeaponBarrel UMETA(DisplayName="Weapon Barrel"),
	Head UMETA(DisplayName="Head"),
	Chest UMETA(DisplayName="Chest"),
	Legs UMETA(DisplayName="Legs"),
};

USTRUCT( BlueprintType )
struct FWeaponFX : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX") USoundCue* Shoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX") USoundCue* Reload;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX") USoundCue* DryFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponFX") USoundCue* Equip;
};

USTRUCT( BlueprintType )
struct FWeaponZoomLevel : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") float ZoomFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") float ZoomForward;
	
};

USTRUCT( BlueprintType )
struct FWeaponZoom : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") float ZoomFOVStill;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") float ZoomFOVMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") UTexture2D* ScopeTexture;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") bool bHasMagnifier;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponZoom") TArray<FWeaponZoomLevel> ScopeMagnifierLevels;
	
};

USTRUCT( BlueprintType )
struct FWeaponAccuracy : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAccuracy") FVector2D RecoilPitchRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAccuracy") FVector2D RecoilYawRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAccuracy") FVector2D SpreadPitchRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAccuracy") FVector2D SpreadYawRange;
	// ((ShotsFired-1) * Factor * Recoil) + Recoil 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAccuracy") float IncreaseRecoilPerShotFactor = 1.f;
	// ((ShotsFired-1) * Factor * Spread) + Spread 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponAccuracy") float IncreaseSpreadPerShotFactor = 1.f;
};

USTRUCT( BlueprintType )
struct FWeaponVisual  : public FTableRowBase{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visuals") USkeletalMesh* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visuals") FTransform Offset;
};

USTRUCT( BlueprintType )
struct FWeaponProperties : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FText WeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FName AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") EFireMode FireMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") int32 MagazineCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") float RateOfFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") float ReloadDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") float EquipDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") float WeaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") float WeaponDamageToEnvironmentChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FWeaponAccuracy WeaponAccuracyHipfire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FWeaponAccuracy WeaponAccuracyADS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FWeaponZoom WeaponZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") bool bIsRaycast = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") float RaycastLength = 100000000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") uint8 RoundsPerShot = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") TSubclassOf<ABulletBase> WeaponProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") UTexture2D* WeaponIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FWeaponFX WeaponFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties") FWeaponVisual WeaponVisuals;
};

USTRUCT( BlueprintType )
struct FItemToWeaponAssociation : public FTableRowBase {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item to Weapon Association") FWeaponProperties Weapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item to Weapon Association") FTransform WeaponModelOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item to Weapon Association") USkeletalMesh* WeaponModel;
};


//todo put these elsewhere
USTRUCT( BlueprintType )
struct FMobPack {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") TSubclassOf<AMobEntityBase> MobType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") uint8 Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") float Delay;
};

USTRUCT( BlueprintType )
struct FMobWave {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") TArray<FMobPack> Packs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") float InitialDelay;
};

USTRUCT( BlueprintType )
struct FMobInvasion  : public FTableRowBase{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") TArray<FMobWave> Waves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mob Pack") float InitialDelay;
};


