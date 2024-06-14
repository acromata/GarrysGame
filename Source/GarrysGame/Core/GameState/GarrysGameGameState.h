#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GarrysGame/Core/GameInstance/GarrysGame_GameInstance.h"
#include "GarrysGameGameState.generated.h"

UENUM(BlueprintType)
enum ETimerEnum
{
	// Minigame Timers
	TimerPreGame,
	TimerDuringGame,
	TimerPostGame,
	TimerNull,
	
	// Lobby Timers
	LTimerNotReady,
	LTimerAllReady
};

UCLASS()
class GARRYSGAME_API AGarrysGameGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	// Network Updates
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	virtual void BeginPlay() override;

	// Timer
	UFUNCTION(BlueprintCallable)
	int32 GetTimeFromTimerEnum();

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 PreGameTimerLength;
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 PostGameTimerLength;
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 NotReadyLobbyTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 PlayersReadyLobbyTimer;

	UPROPERTY(Replicated)
	int32 CurrentTimerTime;
	UPROPERTY(Replicated)
	TEnumAsByte<ETimerEnum> CurrentTimerEnum;

	// Levels
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<ULevelData*> Levels;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* LobbyLevelData;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* WinLevelData;

	UPROPERTY(EditDefaultsOnly, Category = "Nugget")
	UItemData* NuggetItem;

	class AMainGameMode* MainGameMode;

	UPROPERTY(BlueprintReadOnly)
	UGarrysGame_GameInstance* GameInstance;

public:

	// Timer
	UFUNCTION(BlueprintCallable)
	int32 SubtractTime();

	UFUNCTION(BlueprintCallable)
	void SetTimerTime(int32 Time) { CurrentTimerTime = Time; }

	UFUNCTION(BlueprintCallable)
	int32 GetTimerTime() const { return CurrentTimerTime; }

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<ETimerEnum> GetTimerEnum() const { return CurrentTimerEnum; }

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<ETimerEnum> MoveToNextTimerType();

	UFUNCTION(BlueprintCallable)
	void SetTimerType(TEnumAsByte<ETimerEnum> TimerType);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTimerChange();

	// Levels
	UFUNCTION(BlueprintCallable)
	ULevelData* GetLobbyData() const { return LobbyLevelData; }

	UFUNCTION(BlueprintCallable)
	ULevelData* GetWinLevelData() const { return WinLevelData; }

	UFUNCTION(BlueprintCallable)
	TArray<ULevelData*> GetLevels() const { return Levels; }
};
