#include "../GameState/GarrysGameGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Containers/Array.h"

void AGarrysGameGameState::BeginPlay()
{
	Super::BeginPlay();

	// Timer
	CurrentTimerTime = PreGameTimerLength;

	// Game Instance
	GameInstance = Cast<UGarrysGame_GameInstance>(GetGameInstance());

	// Game Mode
	MainGameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
}

void AGarrysGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGarrysGameGameState, PlayerCount);
	DOREPLIFETIME(AGarrysGameGameState, PlayersConnected);
	DOREPLIFETIME(AGarrysGameGameState, PlayersReady);
	DOREPLIFETIME(AGarrysGameGameState, NumOfPlayersReady);
	DOREPLIFETIME(AGarrysGameGameState, LevelToOpen);
	DOREPLIFETIME(AGarrysGameGameState, CurrentTimerTime);
	DOREPLIFETIME(AGarrysGameGameState, CurrentTimerEnum);
}

#pragma region Players

void AGarrysGameGameState::OnPlayerLogin(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Add(Player);
		PlayerCount++;

		// Check if in lobby
		FString CurrentLevelName = GetWorld()->GetMapName();
		CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
		if (CurrentLevelName == LobbyLevelData->GetLevelName())
		{
			Player->SetEquippedItem(NuggetItem);
			GameInstance->SetCurrentLevel(LobbyLevelData);
			Player->EnablePlayerInput();

			GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, "Joined Lobby");
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, ("Joined %s", CurrentLevelName));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Green, FString::Printf(TEXT("Player Joined. Count: %f"), PlayerCount));
		UE_LOG(LogTemp, Warning, TEXT("Player Joined. Count: %f"), PlayerCount);
	}
}

void AGarrysGameGameState::OnPlayerLogout(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Remove(Player);
		PlayerCount--;

		//GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Green, FString::Printf(TEXT("Player Left. Count: %f"), PlayerCount));
		UE_LOG(LogTemp, Warning, TEXT("Player Left. Count: %f"), PlayerCount);

		if (PlayerCount == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Players gone, Restarting lobby"));
			ReturnToLobby();
		}
	}
}

void AGarrysGameGameState::OnPlayerDeath_Implementation()
{
	if (GetNumOfAlivePlayers() <= 1)
	{
		OnGameEnd();
	}
}

int32 AGarrysGameGameState::GetNumOfAlivePlayers()
{
	GetPlayersConnected(); // removes invalid players
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

TArray<APlayerCharacter*> AGarrysGameGameState::GetPlayersConnected()
{
	for (APlayerCharacter* Player : PlayersConnected)
	{
		if (!IsValid(Player))
		{
			PlayersConnected.Remove(Player);
			PlayerCount--;
		}
	}

	return PlayersConnected;
}

void AGarrysGameGameState::AddPlayerReady(APlayerCharacter* Player)
{
	NumOfPlayersReady++;
	PlayersReady.Add(Player);
}

#pragma endregion

#pragma region Levels

void AGarrysGameGameState::ReturnToLobby()
{
	SetLevelToOpen(LobbyLevelData);
	UE_LOG(LogTemp, Warning, TEXT("Returning to lobby"));
}

void AGarrysGameGameState::OpenRandomLevel()
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

void AGarrysGameGameState::SetLevelToOpen(ULevelData* LevelData)
{
	if (IsValid(LevelData))
	{
		UE_LOG(LogTemp, Warning, TEXT("Opening level %s"), *LevelToOpen);

		LevelToOpen = LevelData->GetLevelName();
		OpenNewLevel();
		GameInstance->SetCurrentLevel(LevelData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Level Invalid"));
	}
}

//void AGarrysGameGameState::Server_SetLevelToOpen_Implementation(ULevelData* LevelData)
//{
//	if (IsValid(LevelData))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Opening level %s"), *LevelToOpen);
//
//		LevelToOpen = LevelData->GetLevelName();
//		OpenLevel();
//		GameInstance->SetCurrentLevel(LevelData);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Level Invalid"));
//	}
//}

void AGarrysGameGameState::OnGameEnd_Implementation()
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

#pragma endregion

#pragma region Timer

int32 AGarrysGameGameState::GetTimeFromTimerEnum()
{
	int32 MinigameTimeLength;
	if (IsValid(GameInstance) && IsValid(GameInstance->GetCurrentLevel()) && IsValid(GameInstance->GetCurrentLevel()->GetMinigameData()))
	{
		MinigameTimeLength = GameInstance->GetCurrentLevel()->GetMinigameData()->GetMinigameTime();
	}
	else
	{
		MinigameTimeLength = 3;
	}

	switch (CurrentTimerEnum)
	{
	case TimerPreGame:
		return PreGameTimerLength;
		break;
	case TimerDuringGame:
		return MinigameTimeLength;
		break;
	case TimerPostGame:
		return PostGameTimerLength;
		break;
	default:
		return 999;
	}
}

TEnumAsByte<ETimerEnum> AGarrysGameGameState::MoveToNextTimerType()
{
	switch (CurrentTimerEnum)
	{
	case TimerPreGame:
		CurrentTimerEnum = ETimerEnum::TimerDuringGame;
		CurrentTimerTime = GetTimeFromTimerEnum();
		break;
	case TimerDuringGame:
		CurrentTimerEnum = ETimerEnum::TimerPostGame;
		CurrentTimerTime = GetTimeFromTimerEnum();
		break;
	default:
		CurrentTimerEnum = ETimerEnum::TimerNull;
		CurrentTimerTime = 999;
	}

	return CurrentTimerEnum;
}

#pragma endregion

void AGarrysGameGameState::GiveRandomPlayerItem_Implementation(UItemData* Item)
{
	int32 RandNum = FMath::RandRange(0, PlayersReady.Num() - 1);
	if (PlayersReady.IsValidIndex(RandNum) && IsValid(PlayersReady[RandNum]))
	{
		PlayersReady[RandNum]->SetEquippedItem(Item);
	}
}