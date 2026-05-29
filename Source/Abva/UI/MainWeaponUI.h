

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWeaponUI.generated.h"

class AWeaponBase;


UCLASS(Abstract)
class UMainWeaponUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindToWeapon(AWeaponBase* weapon);

	void UpdateCrosshairSpread(float spread);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponShot(AWeaponBase* weapon);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponReloadStart(AWeaponBase* weapon);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponReloadEnd(AWeaponBase* weapon);

	UFUNCTION(BlueprintImplementableEvent)
	void SetUpWeapon(AWeaponBase* weapon);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCrosshairSpreadInPixels(float pixelSpread);

private:
	TWeakObjectPtr<AWeaponBase> Weapon;

	UFUNCTION()
	void OnWeaponShotEvent();

	UFUNCTION()
	void OnWeaponReloadStartEvent();

	UFUNCTION()
	void OnWeaponReloadEndEvent();
};
