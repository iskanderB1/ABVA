#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemActivatedEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDeactivatedEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDisownedEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemOwnedEvent);

class UItemManager;

UCLASS(ClassGroup = (custom), meta = (BlueprintSpawnableComponent))
class UItem : public UActorComponent
{
private:
	GENERATED_BODY()

	UItem();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default values")
	UItemManager* Owner;

public:
	void PickItem(UItemManager* NewOwner);

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOwnerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
	void ActivateItem() const;
	UPROPERTY(BlueprintAssignable)
	FOnItemActivatedEvent ItemActivated;

	void DeactivateItem() const;
	UPROPERTY(BlueprintAssignable)
	FOnItemDeactivatedEvent ItemDeactivated;

	void DisownItem();
	UPROPERTY(BlueprintAssignable)
	FOnItemDisownedEvent ItemDisowned;

	UPROPERTY(BlueprintAssignable)
	FOnItemOwnedEvent ItemOwned;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform AttachTransform;

public:
	void MainInteract() const;
	TDelegate<void()> OnMainInteract;
	
	void StopMainInteract() const;
	TDelegate<void()> OnStopMainInteract;

	void SecondaryInteract() const;
	TDelegate<void()> OnSecondaryInteract;

	void StopSecondaryInteract() const;
	TDelegate<void()> OnStopSecondaryInteract;

	void TertiaryInteract() const;
	TDelegate<void()> OnTertiaryInteract;

	void StopTertiaryInteract() const;
	TDelegate<void()> OnStopTertiaryInteract;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default values")
	FVector AttachedOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default values")
	bool IsPickable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemType = FName("Item");

	void ResetPickableState();
		
	UFUNCTION(BlueprintCallable)
	AActor* GetActorOwner() const;

	UFUNCTION(BlueprintCallable) 
	bool IsOwned() const { return Owner != nullptr; }

};