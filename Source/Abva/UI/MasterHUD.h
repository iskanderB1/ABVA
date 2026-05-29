

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
 

    void TogglePauseMenu(bool bIsPaused);
	
	
};
