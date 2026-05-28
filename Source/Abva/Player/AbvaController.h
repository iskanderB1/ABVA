#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "AbvaController.generated.h"
class AAbvaPlayer;
UCLASS()
class AAbvaController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	AAbvaController();
public:

protected:
	AAbvaPlayer* GetControlledPlayer();

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);
	virtual FGenericTeamId GetGenericTeamId() const;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	UPROPERTY(EditAnywhere)
	uint8 Team = 200;
};
