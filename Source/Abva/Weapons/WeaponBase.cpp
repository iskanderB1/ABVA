// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "WeaponLogicComponent/WeaponLogicComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Abva/Item/Item.h"
#include "Abva/Item/ItemManager.h"
#include "WeaponData.h"

AWeaponBase::AWeaponBase()
{
	WeaponLogic = CreateDefaultSubobject<UWeaponLogicComponent>(FName("WeaponLogic"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("WeaponMesh"));
	PickBoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BoxCollision"));
	Item = CreateDefaultSubobject<UItem>(FName("Item"));
	
	SetRootComponent(WeaponMesh);
	
	Item->ItemType = FName("Weapon");
}
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	Item->OnMainInteract.BindUObject(this, &AWeaponBase::Shoot);
	Item->OnStopMainInteract.BindUObject(this, &AWeaponBase::StopShooting);
	Item->OnTertiaryInteract.BindUObject(this, &AWeaponBase::Reload);
	Item->ItemOwned.AddDynamic(this, &AWeaponBase::OnItemPicked);
	Item->ItemDisowned.AddDynamic(this, &AWeaponBase::OnItemThrown);
	
}

void AWeaponBase::Tick(float deltaTime)
{
	if (IsAuto)
	{
		Shoot();
	}
}

void AWeaponBase::Shoot()
{
	FVector start;
	FRotator dir;
	Item->GetActorOwner()->GetActorEyesViewPoint(start, dir);
	WeaponLogic->SemiShoot(start, dir.Vector());
}

void AWeaponBase::ShootOnDir(const FVector& start, const FVector& dir)
{
	WeaponLogic->SemiShoot(start, dir);
}

void AWeaponBase::StopShooting()
{
	WeaponLogic->EndAutoShoot();
}

void AWeaponBase::Reload()
{
	WeaponLogic->Reload();
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	auto box = WeaponMesh->Bounds.BoxExtent;
	//auto box = WeaponMesh->GetLocalBounds().BoxExtent;
	PickBoxCollision->SetBoxExtent(box * 2);
	PickBoxCollision->SetRelativeLocation(WeaponMesh->Bounds.Origin);
}

void AWeaponBase::ItemAlreadyExists_Implementation(UItem* duplicateItem)
{
	check(duplicateItem->GetOwner()->GetClass()->IsChildOf(GetClass()));
	
	auto weapon = CastChecked<AWeaponBase>(duplicateItem->GetOwner());
	auto data = weapon->WeaponLogic->WeaponData;
	weapon->WeaponLogic->CurrentAmmo = FMath::Max(data->MagAmmo, weapon->WeaponLogic->CurrentAmmo + data->MagAmmo);
	weapon->Destroy();
}
void AWeaponBase::OnItemPicked()
{
	auto owner = Item->GetActorOwner();
	OnWeaponPicked(owner);
}

void AWeaponBase::OnWeaponPicked(const AActor* newOwner)
{
}
void AWeaponBase::OnItemThrown()
{

}
void AWeaponBase::Destroyed()
{
	Super::Destroyed();
}

AActor* AWeaponBase::GetActorOwner() const
{
	return Item->GetActorOwner();
}

bool AWeaponBase::DoesPlayerOwnWeapon() const
{
	if (auto pawn = Cast<APawn>(GetActorOwner()))
	{
		return pawn->GetController() == GetWorld()->GetFirstPlayerController();
	}
	return false;
}
