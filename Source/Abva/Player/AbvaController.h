#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "Abva/Weapons/CrosshairOffsetProvider.h"
#include "AbvaController.generated.h"

class AAbvaPlayer;
class AWeaponBase;
class UMainWeaponUI;

UCLASS()
class AAbvaController : public APlayerController, public IGenericTeamAgentInterface, public ICrosshairOffsetProvider
{
	GENERATED_BODY()
	AAbvaController();
public:
	virtual void BeginPlay() override;

	virtual void UpdateCrosshairSpread(float spread) override;

protected:

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);
	virtual FGenericTeamId GetGenericTeamId() const;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	UPROPERTY(EditAnywhere)
	uint8 Team = 200;
protected:
	UFUNCTION()
	virtual void OnWeaponPicked(int idx);
	void AddWeaponUI(AWeaponBase* currentWeapon);

	//meant to keep a strong reference to UI, thus saving it from being GCed
	//it's not intended to be used directly
	UPROPERTY()
	TObjectPtr<UMainWeaponUI> WeaponUICache;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UMainWeaponUI> BaseWeaponUIClass;
	UPROPERTY()
	TWeakObjectPtr<UMainWeaponUI> ActiveUI;
private:
	auto GetControlledPlayer() const;

};
