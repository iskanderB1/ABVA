#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Abva/Item/ItemInterface.h"

#include "WeaponLogicComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShootEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadStartEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadEndEvent);

class UWeaponData;

UCLASS(BlueprintType)
class UWeaponLogicComponent : public UActorComponent
{
	GENERATED_BODY()

	UWeaponLogicComponent();
	friend class AWeaponBase;
public:

	//the current ammo of this gun. technically it doesnt make sense for it to be above MagAmmo value.
	//But you can go crazy with it i dont care :P
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAmmo = 30;
	//The ammo in the user's "pocket". this is the what the currentAmmo pulls from
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ReserveAmmo = 30;

	//can we shoot his weapon? use this if you want this gun to be used for cosmatic reasons only. 
	//though it would make more sense if you just use a static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsShootable = true;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UWeaponData> WeaponData;

public:
	UPROPERTY(BlueprintAssignable)
	FShootEvent OnShoot;

	UPROPERTY(BlueprintAssignable)
	FReloadStartEvent OnReloadStart;

	UPROPERTY(BlueprintAssignable)
	FReloadEndEvent OnReloadEnd;


private:

	double LastShotTimeStamp = -1;

	FTimerHandle ShootHandle;

	void Shoot(const FVector& start, const FVector& dir);

	void SemiShoot(const FVector& start, const FVector& dir);

	void StartAutoShoot(const USceneComponent* const eyeView);

	void EndAutoShoot();

	void Reload();

	AWeaponBase* GetWeaponOwner() const
	{
		return CastChecked<AWeaponBase>(GetOwner());
	}
};