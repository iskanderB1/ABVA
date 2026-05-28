#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemManager.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedEvent, int, ItemIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbandonedEvent, UItem*, AbandonedItem);

class UItem;
USTRUCT(BlueprintType)
struct FItems
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<UItem*> Items = TArray<UItem*>();
	FItems(const TArray<UItem*>& srs)
		: Items(srs)
	{
	}

	FItems(TArray<UItem*>&& srs)
		: Items(MoveTemp(srs))
	{
	}
	FItems() = default;

};
UCLASS(ClassGroup = (custom), meta = (BlueprintSpawnableComponent))
class UItemManager : public UActorComponent
{
	GENERATED_BODY()

	UItemManager();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item data")
	int CurrentItemIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item data")
	UItem* CurrentItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item data")
	FName CurrentItemType;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Data")
	TMap<FName, FItems> OwnedItemMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item data")
	USceneComponent* ComponentToAttachTo;

	UPROPERTY(BlueprintAssignable)
	FOnEquippedEvent ItemEquippedEvent;

	UPROPERTY(BlueprintAssignable)
	FOnAbandonedEvent ItemAbandonedEvent;

	//version for if the current Item IS simulating physics
	UFUNCTION(BlueprintCallable)
	void ThrowItem(const FVector& VelocityOffset, const FVector& ThrowDir);

	//version for if the current Item is NOT simulating physics. 
	//The Caller is expected to handle how to "dispose" of the item if destroy == false
	UFUNCTION(BlueprintCallable)
	void DiscardItem(bool destroy = true);

	UFUNCTION(BlueprintCallable)
	bool ChangeItem(int IndexOffset);
	
	UFUNCTION(BlueprintCallable)
	void AddItem(UItem* Item);

	UFUNCTION(BlueprintCallable)
	void SetUpManager(USceneComponent* CompToAttachTo);

	UFUNCTION(BlueprintCallable)
	inline void RegisterItemType(FName ItemType)
	{
		if (!OwnedItemMap.Contains(ItemType))
		{
			OwnedItemMap.Add(ItemType, FItems());
		}
	}

public:

	UFUNCTION(BlueprintCallable)
	AActor* GetActiveItem();

	UFUNCTION(BlueprintCallable)
	void GiveItem(TSubclassOf<AActor> itemClass);

private:

	UFUNCTION(BlueprintCallable)
	inline TArray<UItem*>& GetActiveItemArray()
	{
		return OwnedItemMap[CurrentItemType].Items;
	}
	inline void AddUniqueType(FName ItemType)
	{
		if (!OwnedItemMap.Contains(ItemType))
		{
			OwnedItemMap.Add(ItemType, TArray<UItem*>());
		}
	}
};