// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AIBaseController.generated.h"

USTRUCT(BlueprintType)
struct FPatrolPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;
};

class AAIBase;
class UItemManager;
struct FAIStimulus;
UCLASS()
class ABVA_API AAIBaseController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIBaseController();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
	UAIPerceptionComponent* Perception;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FPatrolPoint> PatrolPoints;

	enum class ABVATeam : uint8
	{
		AITeam = 100,
		PlayerFriendlies = 101,
		Player = 200
	};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	protected:
	AAIBase* GetControlledAgent() const;
protected:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);
	virtual FGenericTeamId GetGenericTeamId() const;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const;
	UPROPERTY(EditAnywhere)
	uint8 Team = static_cast<uint8>(ABVATeam::AITeam);

protected:
	UFUNCTION()
	void PerceptionUpdated(AActor* actor, FAIStimulus stimuli);


};
