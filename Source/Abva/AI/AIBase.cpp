// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBase.h"

#include "Abva/HealthManager/HealthManager.h"
#include "Abva/Item/ItemManager.h"

// Sets default values
AAIBase::AAIBase()
{
	PrimaryActorTick.bCanEverTick = true;
	HealthManager = CreateDefaultSubobject<UHealthManager>(FName("HealthManager"));
	ItemManager = CreateDefaultSubobject<UItemManager>(FName("ItemManager"));
}

// Called when the game starts or when spawned
void AAIBase::BeginPlay()
{
	Super::BeginPlay();
	HealthManager->OnDeath.AddDynamic(this, &AAIBase::OnDeathReceived);
}

// Called every frame
void AAIBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIBase::OnDeathReceived(const FDamageInfo& DamangeInfo)
{
	//Do death routine here.
}
