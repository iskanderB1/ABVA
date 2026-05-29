#include "AbvaController.h"
#include "AbvaPlayer.h"
#include "Abva/Weapons/WeaponBase.h"
#include "Abva/Weapons/MainWeaponUI.h"
#include "Abva/Item/ItemManager.h"

AAbvaController::AAbvaController()
{

}

auto AAbvaController::GetControlledPlayer() const
{
	return CastChecked<AAbvaPlayer>(GetPawn());
}

void AAbvaController::BeginPlay()
{
	Super::BeginPlay();

	auto player = GetControlledPlayer();
	player->ItemManager->ItemEquippedEvent.AddDynamic(this, &AAbvaController::OnWeaponPicked);
}

void AAbvaController::UpdateCrosshairSpread(float spread)
{
	if (ActiveUI.IsValid())
	{
		ActiveUI.Get()->UpdateCrosshairSpread(spread);
	}
}

void AAbvaController::OnWeaponPicked(int idx)
{
	auto player = GetControlledPlayer();
	auto item = player->ItemManager->GetActiveItem();

	if (auto weapon = Cast<AWeaponBase>(item))
	{
		AddWeaponUI(weapon);
	}
}
void AAbvaController::AddWeaponUI(AWeaponBase* currentWeapon)
{
	check(IsValid(currentWeapon));

	if (!IsValid(currentWeapon->WidgetCache))
	{
		auto& widgetClass = currentWeapon->UIData.WeaponWidgetClass;

		//User didnt want to specify any unique UI. just display the generic Weapon UI
		if (!IsValid(widgetClass))
		{
			if (!IsValid(WeaponUICache))
			{
				check(BaseWeaponUIClass);
				
				WeaponUICache = CreateWidget<UMainWeaponUI>(this, BaseWeaponUIClass);
				ActiveUI = WeaponUICache;
			}

			WeaponUICache->BindToWeapon(currentWeapon);
			WeaponUICache->AddToViewport();
			return;
		}

		currentWeapon->WidgetCache = CreateWidget<UMainWeaponUI>(this, widgetClass);
		ActiveUI = currentWeapon->WidgetCache;
	}

	currentWeapon->WidgetCache->BindToWeapon(currentWeapon);
	currentWeapon->WidgetCache->AddToViewport();
}

void AAbvaController::SetGenericTeamId(const FGenericTeamId& newTeamID)
{
	Team = newTeamID.GetId();
}

FGenericTeamId AAbvaController::GetGenericTeamId() const
{
	return FGenericTeamId(Team);
}

ETeamAttitude::Type AAbvaController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (auto pawn = Cast<APawn>(&Other))
	{
		if (auto team = Cast<IGenericTeamAgentInterface>(pawn->GetController()))
		{
			auto id = team->GetGenericTeamId();

			if (id == GetGenericTeamId())
			{
				return ETeamAttitude::Friendly;
			}
			else if (id == FGenericTeamId::NoTeam)
			{
				return ETeamAttitude::Neutral;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}
	}
	return ETeamAttitude::Neutral;
}
