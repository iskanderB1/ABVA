#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthManager.generated.h"

USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HitBone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DamageSource;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* DamageCauser;

	explicit inline FDamageInfo(FName hitBone, float damageAmount, FVector damageSource, FVector impactPoint, AActor* damageCauser);
	FDamageInfo() {};
};

inline FDamageInfo::FDamageInfo(FName hitBone, float damageAmount, FVector damageSource, FVector impactPoint, AActor* damageCauser)
	: HitBone(hitBone), DamageAmount(damageAmount), DamageSource(damageSource), ImpactPoint(impactPoint), DamageCauser(damageCauser)
{}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, const FDamageInfo&, DamageInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, const FDamageInfo&, DamageInfo);

UCLASS(ClassGroup = (custom), meta = (BlueprintSpawnableComponent))
class UHealthManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health;

	void Damage(const FDamageInfo& info);
	void Die(const FDamageInfo& lastDamageInfo) const;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnDamage OnDamage;

private:
	UFUNCTION(BlueprintCallable)
	bool IsDead() const { return Health < 0; }
};