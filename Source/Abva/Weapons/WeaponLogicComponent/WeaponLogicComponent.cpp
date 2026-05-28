#include "WeaponLogicComponent.h"
#include "GameFramework/Character.h"
#include "Abva/Weapons/WeaponBase.h"
#include "Abva/HealthManager/HealthManager.h"
#include "Abva/Item/Item.h"
#include "Abva/UI/WeaponHud.h"
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
	const float panickedShotPenalty = currentTime - LastShotTimeStamp < RecoveryTime ? PanicShootPenality : 0.f;
	LastShotTimeStamp = currentTime;

	const float finalCalculatedPenalty = Accuracy + panickedShotPenalty;
	auto randDir = FMath::VRandCone(dir, finalCalculatedPenalty);

	BroadcastAccuracyToHud(finalCalculatedPenalty);

	const FVector end = start + (dir + randDir) * Range / 2;

	if(Dev::TraceDev::DevLineTraceSingleByChannel(GetWorld(), result, start, end, ShootableChannel, params))
	{
		//some actors likes to kill themselves when they're damaged which causes the actor/component to be
		//invalid. so check if it's simulating physics *first* then damage them
		if (result.Component->IsSimulatingPhysics())
		{
			const float ForceScale = 5000.f;
			result.Component->AddForceAtLocation(dir * Damage * ForceScale, result.ImpactPoint, result.BoneName);
		}

		if (auto healthManager = result.GetActor()->GetComponentByClass<UHealthManager>())
		{
			healthManager->Damage(FDamageInfo(result.BoneName, Damage, start, result.ImpactPoint, Owner));
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
		GetWorld()->GetTimerManager().SetTimer(ShootHandle, FireRate, FTimerManagerTimerParameters());
		
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
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, dele, FireRate, FTimerManagerTimerParameters());
}


void UWeaponLogicComponent::Reload()
{
	if (CurrentAmmo <= MagAmmo && MaxAmmo > 0)
	{
		const bool ExtraBullet = CurrentAmmo > 0;
		const float AmountToReFill = MagAmmo - CurrentAmmo;

		if (AmountToReFill > MaxAmmo)
		{
			MaxAmmo = 0;
			CurrentAmmo += AmountToReFill;
		}
		else
		{
			CurrentAmmo += AmountToReFill + ExtraBullet;
			MaxAmmo -= AmountToReFill + ExtraBullet;
		}
		IsShootable = false;
		FTimerHandle timer;
		FTimerManagerTimerParameters params;
		GetWorld()->GetTimerManager().SetTimer(timer, [this] ()
			{
				if(this != nullptr)
					IsShootable = true; 
			}, 2.f, params);
		OnReload.Broadcast();
	}
}

void UWeaponLogicComponent::BroadcastAccuracyToHud(const float newAccuracy) const
{
	if(const auto hud = GetWorld()->GetFirstPlayerController()->GetHUD<AWeaponHud>())
	{
		hud->UpdateAccuracy(newAccuracy);
	}
}
