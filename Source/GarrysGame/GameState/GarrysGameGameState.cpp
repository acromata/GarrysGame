#include "../GameState/GarrysGameGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AGarrysGameGameState::BeginPlay()
{
	Super::BeginPlay();

	// Timer
	CurrentTimerTime = PreGameTimerLength;
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
	GameInstance = Cast<UGarrysGame_GameInstance>(GetGameInstance());
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
		}

		//GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("Player Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerLogout_Implementation(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Remove(Player);
		PlayerCount--;

		//GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("Player Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerDeath_Implementation()
{
	if (GetNumOfAlivePlayers() <= 1)
	{
		OnGameEnd();
	}
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

void AGarrysGameGameState::AddPlayerReady(APlayerCharacter* Player)
{
	NumOfPlayersReady++;
	PlayersReady.Add(Player);
}

#pragma endregion

#pragma region Levels

void AGarrysGameGameState::ReturnToLobby_Implementation()
{
	SetLevelToOpen(LobbyLevelData);
}

void AGarrysGameGameState::SetLevelToOpen_Implementation(ULevelData* LevelData)
{
	if (IsValid(LevelData))
	{
		LevelToOpen = LevelData->GetLevelName();
		OpenLevel();
		GameInstance->SetCurrentLevel(LevelData);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Opening level %s", LevelToOpen));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Level Invalid"));
	}
}

void AGarrysGameGameState::OpenLevel()
{
	GetWorld()->ServerTravel(LevelToOpen);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Server travel called"));
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
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Invalid level");

			SetLevelToOpen(LobbyLevelData);
			GameInstance->SetCurrentLevel(LobbyLevelData);
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