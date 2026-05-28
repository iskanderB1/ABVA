#include "HealthManager.h"

void UHealthManager::Damage(const FDamageInfo& info)
{
	if (IsDead())
	{
		return;
	}
	OnDamage.Broadcast(info);
	Health -= info.DamageAmount;

	if (IsDead())
	{
		Die(info);
	}
}

void UHealthManager::Die(const FDamageInfo& lastDamageInfo) const
{
	OnDeath.Broadcast(lastDamageInfo);
}
