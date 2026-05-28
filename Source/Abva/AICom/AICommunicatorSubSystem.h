#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AICommunicatorSubSystem.generated.h"

class AIBase;
class ICommunicationInterface;
struct FBaseComMessage;


UCLASS()
class UAICommunicatorSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UAICommunicatorSubSystem();

public:
	bool BroadcastWarning(ICommunicationInterface* source, const FBaseComMessage& warning) const;
	void RegisterTeamMember(ICommunicationInterface* newMember);
	TMap<uint32, TArray<ICommunicationInterface*>> Teams;
};