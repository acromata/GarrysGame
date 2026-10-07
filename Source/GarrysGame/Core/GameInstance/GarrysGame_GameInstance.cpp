#include "GarrysGame/Core/GameInstance/GarrysGame_GameInstance.h"
#include "GarrysGame/Core/GameMode/MainGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void UGarrysGame_GameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGarrysGame_GameInstance, CurrentLevelData);
	DOREPLIFETIME(UGarrysGame_GameInstance, PlayersDead);
}
