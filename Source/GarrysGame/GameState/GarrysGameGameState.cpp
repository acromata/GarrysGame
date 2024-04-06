#include "../GameState/GarrysGameGameState.h"
#include "Net/UnrealNetwork.h"

void AGarrysGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGarrysGameGameState, PlayerCount);
	DOREPLIFETIME(AGarrysGameGameState, PlayersConnected);
}

void AGarrysGameGameState::OnPlayerLogin_Implementation(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Add(Player);
		PlayerCount++;

		GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("Player Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerLogout_Implementation(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Remove(Player);
		PlayerCount--;

		GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("Player Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerDeath_Implementation()
{
	if (GetNumOfAlivePlayers() <= 0)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGarrysGameGameState::ReturnToLobby, 2.f);
	}
}

void AGarrysGameGameState::ReturnToLobby_Implementation()
{
	GetWorld()->ServerTravel(LobbyMapName);
}

void AGarrysGameGameState::OpenLevel_Implementation()
{
	GetWorld()->ServerTravel(LevelToOpen);
}

FString AGarrysGameGameState::SetLevelToOpen(FString LevelName)
{
	LevelToOpen = LevelName;
	OpenLevel();

	return LevelToOpen;
}

int32 AGarrysGameGameState::GetNumOfAlivePlayers() const
{
	int32 AlivePlayerCount = PlayerCount;
	
	for (APlayerCharacter* Player : PlayersConnected)
	{
		if (Player->GetIsDead())
		{
			AlivePlayerCount--;
		}
	}

	return AlivePlayerCount;
}
