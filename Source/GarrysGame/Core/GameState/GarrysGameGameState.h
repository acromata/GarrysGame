#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
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

class ULevelData;
class UItemData;
class UServerDataSubsystem;
class AMainGameMode;

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
	UPROPERTY(BlueprintReadOnly)
	AMainGameMode* MainGameMode;

	UPROPERTY(BlueprintReadOnly)
	UServerDataSubsystem* ServerDataSubsystem;

public:

	// Game mode
	void SetMainGameMode(AMainGameMode* GM) { MainGameMode = GM; }

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

	void OnOpenedLobby();

	void OnOpenedGame();

	UFUNCTION(BlueprintCallable)
	ULevelData* GetWinLevelData() const { return WinLevelData; }

	UFUNCTION(BlueprintCallable)
	TArray<ULevelData*> GetLevels() const { return Levels; }

	UPROPERTY(BlueprintReadWrite, Replicated)
	ULevelData* CurrentLevelData;

	// Player data
	UPROPERTY(BlueprintReadWrite, Replicated)
	TArray<class APlayerCharacter*> PlayersAliveInGame;
};
