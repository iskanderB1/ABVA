// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseController.h"
#include "AIBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abva/Item/ItemManager.h"

// Sets default values
AAIBaseController::AAIBaseController()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(FName("Perception"));
}

// Called when the game starts or when spawned
void AAIBaseController::BeginPlay()
{
	Super::BeginPlay();
	if (GetControlledAgent()->bStartAi)
	{
		check(IsValid(BehaviorTree));
		RunBehaviorTree(BehaviorTree);
	}

	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AAIBaseController::PerceptionUpdated);
}

// Called every frame
void AAIBaseController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AAIBase* AAIBaseController::GetControlledAgent() const
{
	return CastChecked<AAIBase>(GetPawn());
}

void AAIBaseController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Team = NewTeamID.GetId();
}

FGenericTeamId AAIBaseController::GetGenericTeamId() const
{
	return FGenericTeamId(Team);
}

ETeamAttitude::Type AAIBaseController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (auto pawn = Cast<APawn>(&Other))
	{
		if (auto team = Cast<IGenericTeamAgentInterface>(pawn->GetController()))
		{
			auto id = team->GetGenericTeamId();

			if (id == GetGenericTeamId())
			{
				return ETeamAttitude::Friendly;
			}
			else if (id == FGenericTeamId::NoTeam)
			{
				return ETeamAttitude::Neutral;
			}
			else
			{
				return ETeamAttitude::Hostile;
			}
		}
	}
	return ETeamAttitude::Neutral;
}

void AAIBaseController::PerceptionUpdated(AActor* actor, FAIStimulus stimuli)
{
	if (!GetControlledAgent()->bStartAi)
	{
		return;
	}
	auto BB = BrainComponent->GetBlackboardComponent();
	
	if (!stimuli.WasSuccessfullySensed())
	{
		return;
	}

	if (auto pawn = Cast<APawn>(actor))
	{
		if (auto team = Cast<IGenericTeamAgentInterface>(pawn->GetController()))
		{
			switch (GetTeamAttitudeTowards(*pawn))
			{
				case ETeamAttitude::Hostile:
					BB->SetValueAsObject(FName("Enemy"), pawn);
				break;
				case ETeamAttitude::Friendly:
					//TODO: check on your team-mate. see if they're dead or whatnot
				break;
			}
			
		}
	}
}

