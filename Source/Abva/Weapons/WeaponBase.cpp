// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "WeaponLogicComponent/WeaponLogicComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Abva/Player/AbvaPlayer.h"
#include "Abva/Item/Item.h"
#include "Abva/Item/ItemManager.h"
#include "Abva/UI/WeaponHud.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

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

void AWeaponBase::Shoot()
{
	if (!IsValid(ShooterEye))
	{
		FVector start;
		FRotator dir;
		Item->GetActorOwner()->GetActorEyesViewPoint(start, dir);
		WeaponLogic->SemiShoot(start, dir.Vector());
	}
	else
	{
		WeaponLogic->StartAutoShoot(ShooterEye);
	}
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
	weapon->WeaponLogic->MaxAmmo += 30;
	weapon->Destroy();
}

void AWeaponBase::OnItemPicked()
{
	OnWeaponPicked(Item->GetActorOwner());
	
}

void AWeaponBase::OnWeaponPicked(const AActor* newOwner)
{
	if (auto player = Cast<AAbvaPlayer>(newOwner))
	{
		WeaponLogic->SetComponentTickEnabled(true);
		auto hud = GetWorld()->GetFirstPlayerController()->GetHUD<AWeaponHud>();
		check(hud);

		AttachToComponent(player->WeaponLock, FAttachmentTransformRules::KeepRelativeTransform);
		SetActorRelativeLocation(WeaponAttachOffset);

		hud->IsWeaponPicked = true;
	}
}
void AWeaponBase::OnItemThrown()
{
	auto hud = GetWorld()->GetFirstPlayerController()->GetHUD<AWeaponHud>();
	if (Cast<AAbvaPlayer>(Item->GetActorOwner()) != nullptr)
	{
		WeaponLogic->SetComponentTickEnabled(false);
		hud->IsWeaponPicked = false;
	}

}
void AWeaponBase::Destroyed()
{
	Super::Destroyed();
	
}

AActor* AWeaponBase::GetActorOwner() const
{
	return Item->GetActorOwner();
}