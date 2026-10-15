


#include "Core/GameInstance/ServerDataSubsystem.h"

void UServerDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

}

bool UServerDataSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Subsystem shouldn't be created on clients
	if (IsValid(GetWorld()))
	{
		return GetWorld()->GetNetMode() > NM_Client;
	}

	return true;
	
}
