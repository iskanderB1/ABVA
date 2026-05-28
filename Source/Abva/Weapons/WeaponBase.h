// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Abva/Item/ItemInterface.h"
#include "WeaponBase.generated.h"

class UWeaponLogicComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class UItem;

UCLASS(Blueprintable)
class AWeaponBase : public AActor, public IItemInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	AWeaponBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponLogicComponent* WeaponLogic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UItem* Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* ShooterEye;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* PickBoxCollision;

	UPROPERTY(EditAnywhere)
	FVector WeaponAttachOffset;

	UFUNCTION(BlueprintCallable)
	virtual void Shoot();

	UFUNCTION(BlueprintCallable)
	virtual void ShootOnDir(const FVector& start, const FVector& dir);

	UFUNCTION(BlueprintCallable)
	virtual void StopShooting();

	UFUNCTION(BlueprintCallable)
	virtual void Reload();

private:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void ItemAlreadyExists_Implementation(UItem* duplicateItem) override;

protected:
	UFUNCTION()
	virtual void OnItemPicked();

	UFUNCTION()
	virtual void OnItemThrown();

	virtual void OnWeaponPicked(const AActor* newOwner);

	virtual void Destroyed() override;

public:
	AActor* GetActorOwner() const;
};
