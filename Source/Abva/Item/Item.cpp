#include "Item.h"
#include "ItemManager.h"
#include "ItemInterface.h"


UItem::UItem()
{
}

void UItem::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UItem::OnOwnerBeginOverlap);
}

void UItem::PickItem(UItemManager* NewOwner)
{
	Owner = NewOwner;
	ItemOwned.Broadcast();
}

void UItem::OnOwnerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (auto NewOwner = OtherActor->FindComponentByClass<UItemManager>())
	{
		GetOwner()->OnActorBeginOverlap.Clear();
		if (!NewOwner->OwnedItemMap.Contains(ItemType))
		{
			NewOwner->AddItem(this);
			return;
		}

		for (auto item : NewOwner->OwnedItemMap[ItemType].Items)
		{
			if (item->GetClass() == GetClass())
			{
				if (auto itemInterface = Cast<IItemInterface>(item->GetOwner()))
				{
					itemInterface->ItemAlreadyExists_Implementation(this);
				}
				
				if (item->GetOwner()->Implements<UItemInterface>())
				{
					IItemInterface::Execute_ItemAlreadyExists(item->GetOwner(), this);
				}
				return;
			}
		}

		NewOwner->AddItem(this);
	}
}

void UItem::ActivateItem() const
{
	ItemActivated.Broadcast();
}

void UItem::DeactivateItem() const
{
	ItemDeactivated.Broadcast();
}

//the item should be already detached from the owner actor 
//but the item manager should be valid
void UItem::DisownItem()
{
	Owner = nullptr;
	IsPickable = true;
	FTimerHandle Handle;
	//doing this so the item wont immediately be picked by the player when thrown while looking down
	GetWorld()->GetTimerManager().SetTimer(Handle , this, &UItem::ResetPickableState, 0.7f);

	ItemDisowned.Broadcast();
}

void UItem::MainInteract() const
{
	OnMainInteract.ExecuteIfBound();
}

void UItem::StopMainInteract() const
{
	OnStopMainInteract.ExecuteIfBound();
}

void UItem::SecondaryInteract() const
{
	OnSecondaryInteract.ExecuteIfBound();
}

void UItem::StopSecondaryInteract() const
{
	OnStopSecondaryInteract.ExecuteIfBound();
}

void UItem::TertiaryInteract() const
{
	OnTertiaryInteract.ExecuteIfBound();
}

void UItem::StopTertiaryInteract() const
{
	OnStopTertiaryInteract.ExecuteIfBound();
}

void UItem::ResetPickableState()
{
	if (auto Comp = CastChecked<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		IsPickable = true;
	}
}

AActor* UItem::GetActorOwner() const
{
	return Owner->GetOwner(); 
}
