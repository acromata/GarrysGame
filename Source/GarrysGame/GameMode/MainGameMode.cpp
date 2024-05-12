#include "../GameMode/MainGameMode.h"
#include "../GameState/GarrysGameGameState.h"
#include "../DataAssets/LevelData.h"
#include "../Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

void AMainGameMode::BeginPlay()
{
	MainGameState = Cast<AGarrysGameGameState>(GameState);
	GameInstance = Cast<UGarrysGame_GameInstance>(GetGameInstance());

	// Check for heartbeats
	FTimerHandle HeartbeatTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &AMainGameMode::CheckForMissedHeartbeats, 15.f, true);
}

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

	int32 RandNum = FMath::RandRange(0, (MainGameState->GetLevels().Num() - 1));
	if (IsValid(MainGameState->GetLevels()[RandNum]) && MainGameState->GetLevels().IsValidIndex(RandNum))
	{
		UE_LOG(LogTemp, Warning, TEXT("Called level to open"));
		SetLevelToOpen(MainGameState->GetLevels()[RandNum]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid level, Returning to lobby..."));
		SetLevelToOpen(MainGameState->GetLobbyData());
	}
}

void AMainGameMode::ReturnToLobby()
{
	SetLevelToOpen(MainGameState->GetLobbyData());
	UE_LOG(LogTemp, Warning, TEXT("Returning to lobby"));
}

void AMainGameMode::OnGameEnd()
{
	for (APlayerCharacter* Player : GetConnectedPlayers())
	{
		if (!Player->GetIsDead() && !Player->bIsInWinzone)
		{
			Player->CallDie();
		}
	}

	// Call new map
	if (GetNumOfAlivePlayers() > 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting new minigame"));
		int32 RandNum = FMath::RandRange(0, MainGameState->GetLevels().Num() - 1);
		if (MainGameState->GetLevels().IsValidIndex(RandNum))
		{
			SetLevelToOpen(MainGameState->GetLevels()[RandNum]);
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
		UE_LOG(LogTemp, Warning, TEXT("One player left, game won"));
		SetLevelToOpen(MainGameState->GetWinLevelData());
	}
	else if(GetNumOfAlivePlayers() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No players left, returning to lobby"));
		SetLevelToOpen(MainGameState->GetLobbyData());
	}
}

#pragma region Players

int32 AMainGameMode::GetNumOfAlivePlayers()
{
	int32 AlivePlayerCount = GetNumOfConnectedPlayers();

	for (APlayerCharacter* Player : GetConnectedPlayers())
	{
		if (Player->GetIsDead())
		{
			AlivePlayerCount--;
		}
	}

	return AlivePlayerCount;
}

TArray<APlayerCharacter*> AMainGameMode::GetConnectedPlayers()
{
	TArray<APlayerCharacter*> PlayerCharacters;

	for (TActorIterator<APlayerCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		PlayerCharacters.Add(*ActorItr);
	}

	return PlayerCharacters;
}

void AMainGameMode::AddPlayerReady(APlayerCharacter* Player)
{
	NumOfPlayersReady++;
	PlayersReady.Add(Player);
}

void AMainGameMode::OnPlayerDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died, %d remaining"), GetNumOfAlivePlayers());

	if (GetNumOfAlivePlayers() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("All players dead, game ended"));
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


#pragma region Heartbeats

void AMainGameMode::ReceiveHeartbeat(APlayerCharacter* Player)
{
	if (IsValid(Player))
	{
		MissedHeartbeatsMap.Add(Player, 0);
	}
}

void AMainGameMode::CheckForMissedHeartbeats()
{
	// Iterate over all player controllers
	for (auto& Pair : MissedHeartbeatsMap)
	{
		APlayerCharacter* PlayerChar = Pair.Key;
		int32& MissedHeartbeats = Pair.Value;

		// If player controller hasn't sent a heartbeat, increment missed heartbeats count
		if (MissedHeartbeats < 3)
		{
			MissedHeartbeats++;
		}
		// If player controller has missed too many heartbeats, kick the player
		else
		{
			UGameplayStatics::OpenLevel(PlayerChar, HeartbeatDisconnectMapName);
			MissedHeartbeats = 0; // Reset missed heartbeats count after kicking
		}
	}
}

#pragma endregion