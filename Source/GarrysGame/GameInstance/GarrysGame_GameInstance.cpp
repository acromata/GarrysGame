#include "../GameInstance/GarrysGame_GameInstance.h"
#include "Net/UnrealNetwork.h"

void UGarrysGame_GameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGarrysGame_GameInstance, CurrentLevelData);
}