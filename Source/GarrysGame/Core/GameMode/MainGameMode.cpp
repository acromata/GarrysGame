#include "GarrysGame/Core/GameMode/MainGameMode.h"
#include "GarrysGame/Core/GameState/GarrysGameGameState.h"
#include "GarrysGame/DataAssets/LevelData.h"
#include "GarrysGame/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Core/GameInstance/ServerDataSubsystem.h"

void AMainGameMode::BeginPlay()
{
	if (IsValid(GetGameInstance()))
	{
		ServerDataSubsystem = UGameInstance::GetSubsystem<UServerDataSubsystem>(GetGameInstance());
	}

	if (IsValid(MainGameState = Cast<AGarrysGameGameState>(GameState)))
	{
		MainGameState->SetMainGameMode(this);
	}

	// Check for heartbeats
	FTimerHandle HeartbeatTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &AMainGameMode::CheckForMissedHeartbeats, 15.f, true);
}

void AMainGameMode::OnPlayerStart(APlayerCharacter* Player)
{
	if (HasAuthority())
	{
		// If in lobby
		if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true) == "Lobby")
		{
			Player->SetEquippedItem(LobbyNuggetItem);
			Player->EnablePlayerInput();

			if (IsValid(MainGameState))
			{
				MainGameState->OnOpenedLobby();
			}

			UE_LOG(LogTemp, Warning, TEXT("Player Joined lobby"));
			return;
		}

		// If is in game
		if (IsValid(ServerDataSubsystem) && IsValid(Player->GetPlayerState()))
		{
			FUniqueNetIdRepl UUID = Player->GetPlayerState()->GetUniqueId();
			if (ServerDataSubsystem->PlayerUUIDsAliveInGame.Contains(UUID))
			{
				MainGameState->PlayersAliveInGame.Add(Player);
				UE_LOG(LogTemp, Warning, TEXT("Player joined game"));
				return;
			}
		}

		// Player joined late
		Player->Die(false);
		UE_LOG(LogTemp, Warning, TEXT("Spectator joined game"));
	}
}

void AMainGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	//if (GetNumOfConnectedPlayers() <= 1)
	//{
	//	ReturnToLobby();
	//}
}



void AMainGameMode::SetLevelToOpen(ULevelData* LevelData)
{
	if (IsValid(LevelData))
	{
		LevelToOpen = LevelData->GetLevelName();
		ServerDataSubsystem->CurrentLevelData = LevelData;

		if (IsValid(ServerDataSubsystem))
		{
			ServerDataSubsystem->CurrentLevelData = LevelData;
			// Add player UUIDs
			for (APlayerCharacter* Player : GetAlivePlayers())
			{
				ServerDataSubsystem->PlayerUUIDsAliveInGame.Add(Player->GetPlayerState()->GetUniqueId());
			}
		}

		GetWorld()->ServerTravel(LevelToOpen);
		UE_LOG(LogTemp, Warning, TEXT("Opening level %s"), *LevelToOpen);
	}
}

void AMainGameMode::OpenRandomLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("Called open random level function"));

	int32 RandNum = FMath::RandRange(0, (MainGameState->GetLevels().Num() - 1));
	while (true)
	{
		
		if (IsValid(MainGameState->GetLevels()[RandNum]) && MainGameState->GetLevels().IsValidIndex(RandNum))
		{
			if (MainGameState->GetLevels()[RandNum]->GetMinigameData()->GetMinimumPlayerCount() >= GetNumOfAlivePlayers())
			{
				UE_LOG(LogTemp, Warning, TEXT("Called level to open"));
				SetLevelToOpen(MainGameState->GetLevels()[RandNum]);
				return;
			}
		}
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
			Player->Die();
		}
	}

	// Call new map
	if (GetNumOfAlivePlayers() > 1)
	{
		// Run until valid map is selected
		while (true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Starting new minigame..."));
			int32 RandNum = FMath::RandRange(0, MainGameState->GetLevels().Num() - 1);
			if (MainGameState->GetLevels().IsValidIndex(RandNum))
			{
				SetLevelToOpen(MainGameState->GetLevels()[RandNum]);
				break;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid level, attempting again"));
			}
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

TArray<APlayerCharacter*> AMainGameMode::GetAlivePlayers()
{
	TArray<APlayerCharacter*> AlivePlayers;
	for (APlayerCharacter* Player : GetConnectedPlayers())
	{
		if (!Player->GetIsDead())
		{
			AlivePlayers.Add(Player);
		}
	}

	return AlivePlayers;
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

void AMainGameMode::OnPlayerDeath(APlayerCharacter* Player)
{
	if (IsValid(ServerDataSubsystem))
	{
		MainGameState->PlayersAliveInGame.RemoveSingle(Player);
		UE_LOG(LogTemp, Warning, TEXT("Player died, %d remaining"), GetNumOfAlivePlayers());
	}

	if (GetNumOfAlivePlayers() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("All players dead, game ended"));
		MainGameState->SetTimerType(ETimerEnum::TimerPostGame);
	}

}

APlayerCharacter* AMainGameMode::GiveRandomPlayerItem(UItemData* Item)
{
	APlayerCharacter* Player;
	TArray<APlayerCharacter*> PReady = GetAlivePlayers();
	while (true)
	{
		int32 RandNum = FMath::RandRange(0, PReady.Num() - 1);
		if (PReady.IsValidIndex(RandNum) && IsValid(PReady[RandNum]))
		{
			Player = PReady[RandNum];
			break;
		}
	}

	Player->SetEquippedItem(Item);
	return Player;
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
	// Iterate over all players
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