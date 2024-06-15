#include "GarrysGame/Core/GameState/GarrysGameGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Containers/Array.h"
#include "GarrysGame/Player/PlayerCharacter.h"
#include "GarrysGame/DataAssets/ItemData.h"
#include "GarrysGame/DataAssets/LevelData.h"
#include "GarrysGame/Core/GameMode/MainGameMode.h"

void AGarrysGameGameState::BeginPlay()
{
	Super::BeginPlay();

	// Timer
	if (UGameplayStatics::GetCurrentLevelName(GetWorld(), true) == "Lobby")
	{
		SetTimerType(ETimerEnum::LTimerNotReady);
	}
	else
	{
		SetTimerType(ETimerEnum::TimerPreGame);
	}
	
	// Game Instance
	GameInstance = Cast<UGarrysGame_GameInstance>(GetGameInstance());

	MainGameMode = GetWorld()->GetAuthGameMode<AMainGameMode>();
}

void AGarrysGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGarrysGameGameState, CurrentTimerTime);
	DOREPLIFETIME(AGarrysGameGameState, CurrentTimerEnum);
}

#pragma endregion

#pragma region Timer

int32 AGarrysGameGameState::GetTimeFromTimerEnum()
{
	// Get minigame time from minigame data
	int32 MinigameTimeLength;
	if (IsValid(GameInstance) && IsValid(GameInstance->GetCurrentLevel()) && IsValid(GameInstance->GetCurrentLevel()->GetMinigameData()))
	{
		MinigameTimeLength = GameInstance->GetCurrentLevel()->GetMinigameData()->GetMinigameTime();
	}
	else
	{
		MinigameTimeLength = 62;
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