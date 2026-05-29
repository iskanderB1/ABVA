// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Abva/Item/ItemInterface.h"
#include "WeaponBase.generated.h"

USTRUCT(Blueprintable)
struct FWeaponUIData
{
	GENERATED_BODY()

	//	Weapon's display name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponName;

	/*
		The crosshair to be used for the weapon
		NOTE: im pretty sure this is a static solution. might want to make this into a material later so it can be more dynamic
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> CrosshairTexture;

	//	the tint colour of our crosshair
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor CrosshairColour;

	/*
		the Unique widget class this weapon has and can be left null
		it is useful if you want to have cool
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WeaponWidgetClass;
};

class UWeaponLogicComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class UItem;
class UMainWeaponUI;

UCLASS(Blueprintable)
class AWeaponBase : public AActor, public IItemInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAuto = false;

public:
	AWeaponBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponLogicComponent> WeaponLogic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UItem> Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> PickBoxCollision;

	UPROPERTY(EditAnywhere)
	FVector WeaponAttachOffset = FVector::ZeroVector;

private:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void ItemAlreadyExists_Implementation(UItem* duplicateItem) override;

protected:

	virtual void Shoot();
	virtual void StopShooting();
	virtual void ShootOnDir(const FVector& start, const FVector& dir);
	virtual void Reload();

	UFUNCTION()
	virtual void OnItemPicked();

	UFUNCTION()
	virtual void OnItemThrown();

	virtual void OnWeaponPicked(const AActor* newOwner);

	virtual void Destroyed() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponUIData UIData;

	UPROPERTY()
	TObjectPtr<UMainWeaponUI> WidgetCache;

public:
	AActor* GetActorOwner() const;
	bool DoesPlayerOwnWeapon() const;
};
