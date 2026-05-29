#include "WeaponLogicComponent.h"
#include "GameFramework/Character.h"
#include "Abva/Weapons/WeaponBase.h"
#include "Abva/HealthManager/HealthManager.h"
#include "Abva/Item/Item.h"
#include "Abva/Weapons/WeaponData.h"
#include "Abva/Weapons/CrosshairOffsetProvider.h"
#include "Abva/Dev/Dev.h"

UWeaponLogicComponent::UWeaponLogicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWeaponLogicComponent::Shoot(const FVector& start, const FVector& dir)
{
	check(IsShootable);

	CurrentAmmo--;

	auto item = GetOwner()->GetComponentByClass<UItem>();
	auto Owner = item == nullptr ? GetOwner() : item->GetActorOwner();

	FHitResult result;
	FCollisionQueryParams params(FName("ShootTrace"), true, Owner);

	const double currentTime = GetWorld()->GetTimeSeconds();
	const float panickedShotPenalty = currentTime - LastShotTimeStamp < WeaponData->RecoveryTime ? WeaponData->PanicShootPenality : 0.f;
	LastShotTimeStamp = currentTime;

	const float finalCalculatedPenalty = WeaponData->Accuracy + panickedShotPenalty;
	auto randDir = FMath::VRandCone(dir, finalCalculatedPenalty);

	//TODO: we calculate the accuracy penalty above
	// of course, we need to reflect that in the UI
	// your job is to figure that out, future me :)
	auto playerController = GetWorld()->GetFirstPlayerController();
	if (GetWeaponOwner()->DoesPlayerOwnWeapon())
	{
		if (auto interface = Cast<ICrosshairOffsetProvider>(playerController))
		{
			interface->UpdateCrosshairSpread(finalCalculatedPenalty);
		}
	}

	const FVector end = start + (dir + randDir) * WeaponData->Range / 2;

	if(Dev::TraceDev::DevLineTraceSingleByChannel(GetWorld(), result, start, end, WeaponData->ShootableChannel, params))
	{
		//some actors like to kill themselves when they're damaged which causes the actor/component to be
		//invalid. so check if it's simulating physics *first* then damage them
		if (result.Component->IsSimulatingPhysics())
		{
			const float ForceScale = WeaponData->ImpactForce;
			result.Component->AddForceAtLocation(dir * WeaponData->Damage * ForceScale, result.ImpactPoint, result.BoneName);
		}

		if (auto healthManager = result.GetActor()->GetComponentByClass<UHealthManager>())
		{
			healthManager->Damage(FDamageInfo(result.BoneName, WeaponData->Damage, start, result.ImpactPoint, Owner));
		}

	}
	OnShoot.Broadcast();
}

void UWeaponLogicComponent::SemiShoot(const FVector& start, const FVector& dir)
{
	if (!IsShootable || CurrentAmmo < 1)
	{
		return;
	}
	if (!GetWorld()->GetTimerManager().IsTimerActive(ShootHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(ShootHandle, WeaponData->FireRate, FTimerManagerTimerParameters());
		
		Shoot(start, dir);
	}
}

void UWeaponLogicComponent::EndAutoShoot()
{
	GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
}

void UWeaponLogicComponent::StartAutoShoot(const USceneComponent* const eyeView)
{
	if (!IsShootable || !IsValid(eyeView) || CurrentAmmo < 1)
	{
		return;
	}

	const FVector& start = eyeView->GetComponentLocation();
	const FVector dir = eyeView->GetComponentQuat().GetForwardVector();
	Shoot(start, dir);

	FTimerDelegate dele;
	dele.BindUObject(this, &UWeaponLogicComponent::StartAutoShoot, eyeView);
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, dele, WeaponData->FireRate, FTimerManagerTimerParameters());
}


void UWeaponLogicComponent::Reload()
{
	if (ReserveAmmo > 0)
	{
		//TODO: maybe we want to make extraBullet feature optional?
		//const bool ExtraBullet = CurrentAmmo > 0;
		const int ammoDif = WeaponData->MagAmmo - CurrentAmmo;
		const int amountToRefill = ammoDif <= WeaponData->MagAmmo ? ammoDif : WeaponData->MagAmmo;

		if (ammoDif == 0)
		{
			return;
		}

		int newCurrentAmmo;
		int newReserveAmmo;

		if (amountToRefill > ReserveAmmo)
		{
			newCurrentAmmo = CurrentAmmo + ReserveAmmo;
			newReserveAmmo = 0;
		}
		else
		{
			newCurrentAmmo = CurrentAmmo + amountToRefill;
			newReserveAmmo = ReserveAmmo - amountToRefill;
		}

		ReserveAmmo += CurrentAmmo;
		CurrentAmmo = 0;

		IsShootable = false;
		FTimerHandle timer;
		FTimerManagerTimerParameters params;
		
		//TODO: reload should be intruppted when the weapon is thrown or changed...ect
		GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateWeakLambda(this, [this, newCurrentAmmo, newReserveAmmo]() 
			{
				IsShootable = true;
				CurrentAmmo = newCurrentAmmo;
				ReserveAmmo = newReserveAmmo;
				OnReloadEnd.Broadcast();
			}), WeaponData->ReloadTime, params);

		OnReloadStart.Broadcast();
	}
}
