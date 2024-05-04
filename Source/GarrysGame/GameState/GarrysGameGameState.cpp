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