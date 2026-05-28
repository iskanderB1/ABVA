#include "AbvaController.h"
#include "AbvaPlayer.h"

AAbvaController::AAbvaController()
{

}

AAbvaPlayer* AAbvaController::GetControlledPlayer()
{
	return CastChecked<AAbvaPlayer>(GetPawn()); 
}

void AAbvaController::SetGenericTeamId(const FGenericTeamId& newTeamID)
{
	Team = newTeamID.GetId();
}

FGenericTeamId AAbvaController::GetGenericTeamId() const
{
	return FGenericTeamId(Team);
}

ETeamAttitude::Type AAbvaController::GetTeamAttitudeTowards(const AActor& Other) const
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
