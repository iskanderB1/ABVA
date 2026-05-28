#include "AICommunicatorSubSystem.h"
#include "Abva/AICom/CommunicationInterface.h"
#include "CommTypes.h"


UAICommunicatorSubSystem::UAICommunicatorSubSystem()
{

}

bool UAICommunicatorSubSystem::BroadcastWarning(ICommunicationInterface* source, const FBaseComMessage& message) const
{
	auto arr = Teams.Find(source->GetTeam());
	check(arr != nullptr);
	for (auto member : *arr)
	{
		member->ReceiveWarning(source, message);
	}
	return true;
}

void UAICommunicatorSubSystem::RegisterTeamMember(ICommunicationInterface* newMember)
{
	const auto team = newMember->GetTeam();
	auto arr = Teams.Find(team);
	if (arr == nullptr)
	{
		Teams.Add(team, TArray<ICommunicationInterface*>({newMember}));
		return;
	}
	arr->Add(newMember);
}

