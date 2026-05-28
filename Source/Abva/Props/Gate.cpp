#include "Gate.h"

AGate::AGate()
{
}

void AGate::Interact_Implementation()
{
	IInteractableItem::Execute_Interact(this);
}
