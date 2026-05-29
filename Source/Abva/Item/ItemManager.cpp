#include "ItemManager.h"
#include "Item.h"

UItemManager::UItemManager()
{
	OwnedItemMap.Add(FName("Item"), TArray<UItem*>());
}



void UItemManager::ThrowItem(const FVector& VelocityOffset, const FVector& ThrowDir)
{
	check(IsValid(CurrentItem))
	
	if (UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(CurrentItem->GetOwner()->GetRootComponent()))
	{
		check(Component->IsSimulatingPhysics());

		CurrentItem->GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		Component->SetSimulatePhysics(true);
		Component->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Component->AddImpulse(VelocityOffset + ThrowDir);
	}

	DiscardItem(false);
	
}

void UItemManager::DiscardItem(bool destroy)
{
	check(IsValid(CurrentItem))

	GetActiveItemArray().RemoveAt(CurrentItemIndex);
	CurrentItem->DisownItem();
	ItemAbandonedEvent.Broadcast(CurrentItem);
	ChangeItem(-1);

	if (destroy)
	{
		CurrentItem->GetActorOwner()->Destroy();
	}

	CurrentItem = nullptr;
}

bool UItemManager::ChangeItem(int IndexOffset)
{	
	if(GetActiveItemArray().Num() > 0)
	{
		CurrentItemIndex += IndexOffset;
		//TODO: i should probably recursive-y this. but tbh i cant be asked
		//TODO: CurrentItemIndex can be *not* current for the type of items we have selected.
		if(CurrentItemIndex > GetActiveItemArray().Num() - 1)
		{
			CurrentItemIndex = 0;
		}
		else if(CurrentItemIndex < 0)
		{
			CurrentItemIndex = GetActiveItemArray().Num() - 1;
		}

		if (CurrentItem)
		{
			CurrentItem->DeactivateItem();
		}

		CurrentItem = GetActiveItemArray()[CurrentItemIndex];
		CurrentItem->ActivateItem();
		ItemEquippedEvent.Broadcast(CurrentItemIndex);
		return true;
	}

	return false;	
}

void UItemManager::AddItem(UItem* Item)
{
	if (Item->IsPickable)
	{
		Item->IsPickable = false;

		AddUniqueType(Item->ItemType);
		OwnedItemMap[Item->ItemType].Items.Add(Item);

		Item->PickItem(this);

		AActor* NewItem = Item->GetOwner();
		check(NewItem->GetRootComponent()->GetClass()->IsChildOf(UPrimitiveComponent::StaticClass()));
		if (auto Comp = CastChecked<UPrimitiveComponent>(NewItem->GetRootComponent()))
		{
			Comp->SetSimulatePhysics(false);
			Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		NewItem->AttachToComponent(ComponentToAttachTo, FAttachmentTransformRules::KeepRelativeTransform);
		NewItem->SetActorRelativeLocation(Item->AttachedOffset);

		if (CurrentItem)
		{
			Item->DeactivateItem();
		}
		else
		{
			CurrentItem = Item;
			CurrentItemType = Item->ItemType;
			Item->ActivateItem();
			ItemEquippedEvent.Broadcast(GetActiveItemArray().Num() - 1);
		}
	}
}

void UItemManager::SetUpManager(USceneComponent* CompToAttachTo)
{
	ComponentToAttachTo = CompToAttachTo;
}

AActor* UItemManager::GetActiveItem()
{
	return GetActiveItemArray().Num() > 0 ? GetActiveItemArray()[0]->GetOwner() : nullptr;
}

void UItemManager::GiveItem(TSubclassOf<AActor> itemClass)
{
	if (auto itemDO = CastChecked<AActor>(itemClass->GetDefaultObject()))
	{
		check(itemDO->GetComponentByClass<UItem>() != nullptr);

		auto item = GetWorld()->SpawnActor(itemClass);
		AddItem(item->GetComponentByClass<UItem>());
	}
}

