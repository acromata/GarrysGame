#include "../GameMode/MainGameMode.h"
#include "../GameState/GarrysGameGameState.h"

void AMainGameMode::SetLevelToOpen(ULevelData* LevelData)
{
	if (IsValid(LevelData))
	{
		LevelToOpen = LevelData->GetLevelName();
		OpenLevel(LevelToOpen);
		GameInstance->SetCurrentLevel(LevelData);

		UE_LOG(LogTemp, Warning, TEXT("Opening level %s"), *LevelToOpen);
	}
}

void AMainGameMode::OpenRandomLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("Called open random level function"));

	int32 RandNum = FMath::RandRange(0, Levels.Num() - 1);
	if (IsValid(Levels[RandNum]) && Levels.IsValidIndex(RandNum))
	{
		UE_LOG(LogTemp, Warning, TEXT("Called level to open"));
		SetLevelToOpen(Levels[RandNum]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid level, Returning to lobby..."));
		SetLevelToOpen(GetLobbyData());
	}
}

void AMainGameMode::ReturnToLobby()
{
	SetLevelToOpen(LobbyLevelData);
	UE_LOG(LogTemp, Warning, TEXT("Returning to lobby"));
}

void AMainGameMode::OnGameEnd()
{
	if (GetNumOfAlivePlayers() > 1)
	{
		int32 RandNum = FMath::RandRange(0, Levels.Num() - 1);
		if (Levels.IsValidIndex(RandNum))
		{
			SetLevelToOpen(Levels[RandNum]);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid level, attempting again"));
			OnGameEnd();
			return;
		}
	}
	else if (GetNumOfAlivePlayers() == 1)
	{
		SetLevelToOpen(WinLevelData);
	}
	else
	{
		SetLevelToOpen(LobbyLevelData);
	}
}

#pragma region Players

int32 AMainGameMode::GetNumOfAlivePlayers()
{
	int32 AlivePlayerCount = NumOfConnectedPlayers;

	for (APlayerCharacter* Player : ConnectedPlayers)
	{
		if (Player->GetIsDead())
		{
			AlivePlayerCount--;
		}
	}

	return AlivePlayerCount;
}

void AMainGameMode::AddPlayerReady(APlayerCharacter* Player)
{
	NumOfPlayersReady++;
	PlayersReady.Add(Player);
}

void AMainGameMode::OnPlayerDeath()
{
	if (GetNumOfAlivePlayers() <= 1)
	{
		OnGameEnd();
	}
}

void AMainGameMode::GiveRandomPlayerItem(UItemData* Item)
{
	int32 RandNum = FMath::RandRange(0, PlayersReady.Num() - 1);
	if (PlayersReady.IsValidIndex(RandNum) && IsValid(PlayersReady[RandNum]))
	{
		PlayersReady[RandNum]->SetEquippedItem(Item);
	}
}

#pragma endregion
