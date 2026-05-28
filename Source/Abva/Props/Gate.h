#pragma once

#include "CoreMinimal.h"
#include "Abva/Interaction/IInteractableItem.h"
#include "Gate.generated.h"

UCLASS()
class AGate : public AActor, public IInteractableItem
{
	GENERATED_BODY()
	AGate();
	virtual void Interact_Implementation() override;
};