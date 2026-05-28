#pragma once
#include "UObject/Interface.h"
#include "IInteractableItem.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UInteractableItem: public UInterface
{
    GENERATED_BODY()
};
    
    
class IInteractableItem
{
    GENERATED_BODY()
    
public:
    
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void Interact();
    virtual void Interact_Implementation() = 0;
};