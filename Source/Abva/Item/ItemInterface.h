#pragma once

#include "CoreMinimal.h"
#include "ItemInterface.generated.h"

class UItem;

UINTERFACE(Blueprintable, MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class IItemInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void ItemAlreadyExists(UItem* duplicateItem);

	virtual void ItemAlreadyExists_Implementation(UItem* duplicateItem) = 0;
};