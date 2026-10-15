#include "GarrysGame/Core/GameState/GarrysGameGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Containers/Array.h"
#include "GarrysGame/Player/PlayerCharacter.h"
#include "GarrysGame/DataAssets/ItemData.h"
#include "GarrysGame/DataAssets/LevelData.h"
#include "GarrysGame/Core/GameMode/MainGameMode.h"
#include "Core/GameInstance/ServerDataSubsystem.h"

void AGarrysGameGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Server data subsystem
		if (IsValid(GetGameInstance()))
		{
			if (IsValid(MainGameMode->ServerDataSubsystem))
			{
				ServerDataSubsystem = MainGameMode->ServerDataSubsystem;
				CurrentLevelData = ServerDataSubsystem->CurrentLevelData;
			}
		}

		// Timer
		SetTimerType(ETimerEnum::TimerPreGame);
	}
}

void AGarrysGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGarrysGameGameState, CurrentTimerTime);
	DOREPLIFETIME(AGarrysGameGameState, CurrentTimerEnum);
	DOREPLIFETIME(AGarrysGameGameState, CurrentLevelData);

	DOREPLIFETIME(AGarrysGameGameState, PlayersAliveInGame);
	DOREPLIFETIME(AGarrysGameGameState, CurrentLevelData);
}

void AGarrysGameGameState::OnOpenedLobby()
{
	PlayersAliveInGame = MainGameMode->GetAlivePlayers();
	CurrentLevelData = GetLobbyData();

	SetTimerType(ETimerEnum::LTimerNotReady);
}

#pragma region Timer

int32 AGarrysGameGameState::GetTimeFromTimerEnum()
{
	// Get minigame time from minigame data
	int32 MinigameTimeLength = 62;
	if (IsValid(CurrentLevelData) && IsValid(CurrentLevelData->GetMinigameData()))
	{
		MinigameTimeLength = CurrentLevelData->GetMinigameData()->GetMinigameTime();
	}

	// Return time based off current enum
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
	case LTimerNotReady:
		return NotReadyLobbyTimer;
	case LTimerAllReady:
		return PlayersReadyLobbyTimer;
	default:
		return 999;
	}
}

int32 AGarrysGameGameState::SubtractTime()
{
	CurrentTimerTime = FMath::Clamp(CurrentTimerTime - 1, 0, 999);
	return CurrentTimerTime;
}

// Called when timer hits 0
TEnumAsByte<ETimerEnum> AGarrysGameGameState::MoveToNextTimerType()
{
	// Sets Current Timer Enum to the next enum, then sets Timer Time
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
	}

	// Blueprint code
	OnTimerChange();

	return CurrentTimerEnum;
}

void AGarrysGameGameState::SetTimerType(TEnumAsByte<ETimerEnum> TimerType)
{
	// Set the enum
	CurrentTimerEnum = TimerType;

	// Set the time
	CurrentTimerTime = GetTimeFromTimerEnum();

	// Blueprint code
	OnTimerChange();
}

#pragma endregion