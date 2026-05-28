#pragma once
#include "CoreMinimal.h"

class ICommunicationInterface;

UENUM(BlueprintType)
enum class AIComWarnings : uint8
{
	Com_None,
	Com_Suspecion,
	Com_EnemySighted,
	Com_EnemyLost
};

enum class AISuspetionReason : uint8
{
	Sus_Noise,
	Sus_EnemySighted,

};

struct FBaseComMessage
{
	AIComWarnings WarningType;
	FVector Location;
	ICommunicationInterface* Source;
	float Distance;
	float TimeStamp;

	FBaseComMessage()
		: WarningType(AIComWarnings::Com_None), Location(FVector::ZeroVector), Source(nullptr), Distance(0.0f), TimeStamp(0.0f) {
	}
	FBaseComMessage(AIComWarnings warning, const FVector& loc, ICommunicationInterface* src, float dist, float time)
		: WarningType(warning), Location(loc), Source(src), Distance(dist), TimeStamp(time) {
	}
};

struct FComSusMessage : public FBaseComMessage
{
	AISuspetionReason SusReason;

	FComSusMessage()
		: SusReason(AISuspetionReason::Sus_Noise) {
	}
	FComSusMessage(AIComWarnings warning, FVector loc, ICommunicationInterface* src, float dist, float time, AISuspetionReason reason)
		: FBaseComMessage{ warning, loc, src, dist, time }, SusReason(reason) {
	}
};

struct FComEnemySightedMessage : public FBaseComMessage
{
	AActor* EnemyActor;

	FComEnemySightedMessage()
		: FBaseComMessage(AIComWarnings::Com_EnemySighted, FVector::ZeroVector, nullptr, 0.0f, 0.0f), EnemyActor(nullptr) {
	}
	FComEnemySightedMessage(AIComWarnings warning, const FVector& loc, ICommunicationInterface* src, AActor* enemy, float dist, float time)
		: FBaseComMessage{ warning, loc, src, dist, time }, EnemyActor(enemy) 
	{}
};

struct FComEnemyLostMessage : public FBaseComMessage
{
	FVector LastKnownLocation;

	FComEnemyLostMessage()
		: FBaseComMessage(AIComWarnings::Com_EnemyLost, FVector::ZeroVector, nullptr, 0.0f, 0.0f), LastKnownLocation(FVector::ZeroVector) {
	}
	FComEnemyLostMessage(AIComWarnings warning, const FVector& loc, ICommunicationInterface* src, const FVector& lastKnownLoc, float dist, float time)
		: FBaseComMessage{ warning, loc, src, dist, time }, LastKnownLocation(lastKnownLoc) {
	}
};