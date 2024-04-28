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

void AGarrysGameGameState::OnPlayerLogin_Implementation(AController* PlayerController)
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
		GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Green, FString::Printf(TEXT("Player Joined. Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerLogout_Implementation(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Remove(Player);
		PlayerCount--;

		GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Green, FString::Printf(TEXT("Player Left. Count: %f"), PlayerCount));

		if (PlayerCount == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Green, "Players gone, restarting lobby");
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
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Returning to lobby"));
}

void AGarrysGameGameState::SetLevelToOpen(ULevelData* LevelData)
{
	if (IsValid(LevelData))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Opening level %s", LevelToOpen));

		LevelToOpen = LevelData->GetLevelName();
		OpenLevel();
		GameInstance->SetCurrentLevel(LevelData);

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Level Invalid"));
	}
}

void AGarrysGameGameState::OpenLevel_Implementation()
{
	GetWorld()->ServerTravel(LevelToOpen);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Open Level Called"));
}

void AGarrysGameGameState::OpenRandomLevel()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Called open random level function");

	int32 RandNum = FMath::RandRange(0, Levels.Num() - 1);
	if (IsValid(Levels[RandNum]) && Levels.IsValidIndex(RandNum))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Called level to open");
		SetLevelToOpen(Levels[RandNum]);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Invalid level, Returning to lobby...");
		SetLevelToOpen(GetLobbyData());
	}
}

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
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Invalid level, attempting again");

			OnGameEnd();
			return;
		}
	}
	else if(GetNumOfAlivePlayers() == 1)
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