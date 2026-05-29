

#include "MainWeaponUI.h"
#include "Abva/Weapons/WeaponBase.h"
#include "Abva/Weapons/WeaponLogicComponent/WeaponLogicComponent.h"

void UMainWeaponUI::BindToWeapon(AWeaponBase* weapon)
{
	check(IsValid(weapon));
	if (Weapon.IsValid())
	{
		Weapon.Get()->WeaponLogic->OnShoot.RemoveDynamic(this, &UMainWeaponUI::OnWeaponShotEvent);
		Weapon.Get()->WeaponLogic->OnReloadStart.RemoveDynamic(this, &UMainWeaponUI::OnWeaponReloadStartEvent);
		Weapon.Get()->WeaponLogic->OnReloadEnd.RemoveDynamic(this, &UMainWeaponUI::OnWeaponReloadEndEvent);
	}
	Weapon = weapon;

	SetUpWeapon(weapon);

	weapon->WeaponLogic->OnShoot.AddDynamic(this, &UMainWeaponUI::OnWeaponShotEvent);
	weapon->WeaponLogic->OnReloadStart.AddDynamic(this, &UMainWeaponUI::OnWeaponReloadStartEvent);
	weapon->WeaponLogic->OnReloadEnd.AddDynamic(this, &UMainWeaponUI::OnWeaponReloadEndEvent);
}

void UMainWeaponUI::UpdateCrosshairSpread(float spread)
{
    auto PC = GetOwningPlayer();

	if (!IsValid(PC) || !IsValid(PC->PlayerCameraManager))
	{
		return;
	}

    float CameraFOV = PC->PlayerCameraManager->GetFOVAngle();
    int32 ViewportSizeX, ViewportSizeY;
    PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

    float HalfFOVRad = FMath::DegreesToRadians(CameraFOV / 2.0f);

    float FocalLength = (ViewportSizeX / 2.0f) / FMath::Tan(HalfFOVRad);
    float PixelOffset = FocalLength * FMath::Tan(spread);

    UpdateCrosshairSpreadInPixels(PixelOffset);
}

void UMainWeaponUI::OnWeaponShotEvent()
{
	if (Weapon.IsValid())
	{
		OnWeaponShot(Weapon.Get());
	}
}

void UMainWeaponUI::OnWeaponReloadStartEvent()
{
	if (Weapon.IsValid())
	{
		OnWeaponReloadStart(Weapon.Get());
	}
}

void UMainWeaponUI::OnWeaponReloadEndEvent()
{
	if (Weapon.IsValid())
	{
		OnWeaponReloadEnd(Weapon.Get());
	}
}
