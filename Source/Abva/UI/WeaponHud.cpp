#include "WeaponHud.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"



AWeaponHud::AWeaponHud()
{
}

void AWeaponHud::UpdateAccuracy(const float newAccuracy)
{
	ProxySpacing = FMath::Clamp(newAccuracy * SpacingModifier, BaseSpacing, MAX_FLT);
	GEngine->AddOnScreenDebugMessage(342934, 5, FColor::Magenta, FString::Printf(TEXT("New accuracy: %f"), newAccuracy), true);
}

void AWeaponHud::DrawHUD()
{
	if (IsWeaponPicked)
	{
		DrawStandardCrosshair();
		return;
	}
	DrawNonWeaponCrosshair();
}

void AWeaponHud::Tick(float deltaTick)
{
	ProxySpacing = FMath::FInterpTo(ProxySpacing, BaseSpacing, deltaTick, 1.f);
	Spacing = FMath::FInterpTo(Spacing, ProxySpacing, deltaTick, 1.f);
	Super::Tick(deltaTick);
}

void AWeaponHud::DrawNonWeaponCrosshair()
{
	const float centerX = (float)Canvas->SizeX / 2;
	const float centerY = (float)Canvas->SizeY / 2;

	FVector2D dir(FVector2D::Unit45Deg);
	FVector2D start(centerX, centerY);
	FVector2D end(centerX, centerY);

	start = start + dir * CrossLength;
	end = end - dir * CrossLength;
	DrawLine(start.X, start.Y, end.X, end.Y, CrosshairColor, CrossThickness);

	dir = dir.GetRotated(90.f);
	start = FVector2D(centerX, centerY);
	end = FVector2D(centerX, centerY);

	start = start + dir * CrossLength;
	end = end - dir * CrossLength;
	DrawLine(start.X, start.Y, end.X, end.Y, CrosshairColor, CrossThickness);
}

void AWeaponHud::DrawCircle()
{
}

void AWeaponHud::DrawStandardCrosshair()
{
	float X, Y;

	const float centerX = (float)Canvas->SizeX / 2;
	const float centerY = (float)Canvas->SizeY / 2;

	X = centerX - CrosshairWidth / 2;
	Y = centerY - CrosshairHeight - Spacing;
	
	// upper crosshair
	DrawRect(CrosshairColor,X, Y, CrosshairWidth, CrosshairHeight);

	// lower crosshair
	Y = centerY + Spacing;
	DrawRect(CrosshairColor, X, Y, CrosshairWidth, CrosshairHeight);

	//right crosshair. NOTE: the height and widths are swaped.
	X = centerX - CrosshairHeight - Spacing;
	Y = centerY - CrosshairWidth / 2;
	DrawRect(CrosshairColor, X, Y, CrosshairHeight, CrosshairWidth);

	//left crosshair
	X = centerX + Spacing;
	DrawRect(CrosshairColor, X, Y, CrosshairHeight, CrosshairWidth);
	
}

void AWeaponHud::DrawRectWithRotation(FVector2D position, FVector2D size, float rotationAngle)
{
}
