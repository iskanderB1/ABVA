#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WeaponHud.generated.h"


UCLASS(Blueprintable, BlueprintType)
class AWeaponHud : public AHUD
{
	GENERATED_BODY()
	AWeaponHud();

public:
	void UpdateAccuracy(const float newAccuracy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float BaseSpacing = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	bool IsWeaponPicked = false;
protected:
	
	virtual void DrawHUD() override;

	virtual void Tick(float deltaTick) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FLinearColor CrosshairColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float CrosshairWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float CrosshairHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Spacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float SpacingModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weaponless")
	float CrossLength = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weaponless")
	float CrossThickness = 3.f;

	float ProxySpacing;
private:
	void DrawCircle();

	void DrawStandardCrosshair();

	void DrawNonWeaponCrosshair();

	void DrawRectWithRotation(FVector2D position, FVector2D size, float rotationAngle);
};
