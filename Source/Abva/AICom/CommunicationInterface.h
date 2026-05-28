#pragma once

#include "CoreMinimal.h"
#include "CommTypes.h"
#include "CommunicationInterface.generated.h"


UINTERFACE(Blueprintable, MinimalAPI)
class UCommunicationInterface : public UInterface
{
	GENERATED_BODY()
};


class ICommunicationInterface
{
	GENERATED_BODY()
public:

	virtual void ReceiveWarning(ICommunicationInterface* source, const FBaseComMessage& warning) const = 0;
	virtual uint32 GetTeam() const = 0;
};