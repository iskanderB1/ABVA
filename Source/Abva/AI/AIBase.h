// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIBase.generated.h"



class UHealthManager;
class UItemManager;
UCLASS()
class ABVA_API AAIBase : public ACharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UHealthManager* HealthManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	UItemManager* ItemManager;

private:
	UFUNCTION()
	void OnDeathReceived(const FDamageInfo& DamangeInfo);
public:
	// Sets default values for this character's properties
	AAIBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
	bool bStartAi = true;
private:
	UPROPERTY(EditAnywhere)
	int TeamAlliance = 2;
};
