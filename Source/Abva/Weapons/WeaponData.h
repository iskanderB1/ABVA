#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"



UCLASS(BlueprintType)
class UWeaponData : public UDataAsset
{
    GENERATED_BODY()
public:
    /*  The base amount of damage the weapon inflicts
        'base' in this case usually means shooting at point-blank
    */
    UPROPERTY(EditAnywhere) float Damage = 30.f;

    //  the time between shots in seconds
    UPROPERTY(EditAnywhere) float FireRate = 0.2f;

    //the max clip size this weapon can have
    UPROPERTY(EditAnywhere) int MagAmmo = 30;

    //Max amount of ammo the player can have
    UPROPERTY(EditAnywhere) int MaxAmmo = 30;

    /*  the time it takes to reload in seconds
        NOTE: this is independent of the animation
    */
    UPROPERTY(EditAnywhere) float ReloadTime = 0.8f;

    //  the force at which this weapon should push physics objects
    UPROPERTY(EditAnywhere) float ImpactForce = 5000.f;

    /*  the max distance(in centimeters) this weapon can inflict damage AT ALL
        anything above and this weapon will do zero damage
    */
    UPROPERTY(EditAnywhere) float Range = 20000.f;

    /*  The amount of time it takes to recover(i.e accuracy is back to normal) after shooting.
        the penelty for now is fixed.
    */
    UPROPERTY(EditAnywhere)
    float RecoveryTime = 0.4f;

    //the penality angle (in radians) we should add for when the player is panic shooting (i.e shooting before the recovery time)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PanicShootPenality = (PI / 180) * 8;

    //the scale of the inaccuracy cone radius.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AccuracyModifier = 1.f;

    //the inaccuracy cone radius (in randians).
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Accuracy = (PI / 180) * 1;

    //what can this gun hit?
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ECollisionChannel> ShootableChannel;

    /*  the distance at which the damage starts to fall off.
        TODO: take in a falloff graph function asset later
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Falloff = 200.f;
};