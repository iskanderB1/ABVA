#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Abva/Item/ItemInterface.h"
#include "WeaponLogicComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadEvent);

UCLASS(BlueprintType)
class UWeaponLogicComponent : public UActorComponent
{
	GENERATED_BODY()

	UWeaponLogicComponent();
	friend class AWeaponBase;

public:
	//The amount of time it takes to recover (i.e accuracy is back to normal) after shooting.
	//the penelty for now is fixed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoveryTime = 0.4f; 

	//the penality angle (in radians) we should add for when the player is panic shooting (i.e shooting before the recovery time)
	//TODO:this value should increase the more the player panic shoots.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PanicShootPenality = (PI/180) * 8;

	//the scale of the inaccuracy cone radius.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccuracyModifier = 1.f; 

	//the inaccuracy cone radius (in randians).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Accuracy = (PI/180) * 1; 

	//the max range of the weapon. anything beyond this distance (in cm) will be ignored.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range = 1000.f; 

	//the distance at which the damage starts to fall off.
	//TODO: take in a falloff graph function asset later
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Falloff = 200.f; 

	//the base damage of the weapon before the falloff
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 30.f; 

	//the fire rate of this weapon. note that this is different from the "recovery time"
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 0.2f;

	//the time it takes to reload the gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReloadTime = 0.8f;

	//the max amount of ammo can the player carry for this gun.
	//NOTE: this is the ammo in reserve, not in the gun's magazine
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxAmmo = 120;

	//the current ammo of this gun. technically it doesnt make sense for it to be above MagAmmo value.
	//But you can go crazy with it i dont care :P
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAmmo = 30;

	//the max amount of ammo the gun can hold.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagAmmo = 30;

	//what can this gun hit?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> ShootableChannel;

	//can we shoot his weapon? use this if you want this gun to be used for cosmatic reasons only. 
	//though it would make more sense if you just use a static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsShootable = true;

protected:
	UPROPERTY(BlueprintAssignable)
	FShootEvent OnShoot;

	UPROPERTY(BlueprintAssignable)
	FReloadEvent OnReload;


private:

	double LastShotTimeStamp = -1;

	FTimerHandle ShootHandle;

	void Shoot(const FVector& start, const FVector& dir);

	void SemiShoot(const FVector& start, const FVector& dir);

	void StartAutoShoot(const USceneComponent* const eyeView);

	void EndAutoShoot();

	void Reload();

	void BroadcastAccuracyToHud(const float newAccuracy) const;

	AWeaponBase* GetWeaponOwner() const
	{
		return CastChecked<AWeaponBase>(GetOwner());
	}
};