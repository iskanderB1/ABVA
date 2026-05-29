

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MasterHUD.generated.h"

/**
 * 
 */
UCLASS()
class ABVA_API AMasterHUD : public AHUD
{
	GENERATED_BODY()
	
private:
    virtual void BeginPlay() override;

public:
    // Widget Classes assigned in Blueprints
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainPlayerHUD> PlayerHUDClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuClass;

    // Actual instantiated widgets
    UPROPERTY()
    UMainPlayerHUD* PlayerHUDWidget;

    UPROPERTY()
    UUserWidget* PauseMenuWidget;

    void TogglePauseMenu(bool bIsPaused);
	
	
};
