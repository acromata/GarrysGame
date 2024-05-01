#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../Player/PlayerCharacter.h"
#include "../DataAssets/ItemData.h"
#include "../DataAssets/LevelData.h"
#include "../GameInstance/GarrysGame_GameInstance.h"
#include "../GameMode/MainGameMode.h"
#include "GarrysGameGameState.generated.h"

UENUM(BlueprintType)
enum ETimerEnum
{
	TimerPreGame,
	TimerDuringGame,
	TimerPostGame,
	TimerNull
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

	// Player
	//UFUNCTION(BlueprintCallable, Server, Reliable)
	//void OnPlayerDeath();

	//UFUNCTION(BlueprintCallable, Server, Reliable)
	//void GiveRandomPlayerItem(UItemData* Item);

	//UFUNCTION(Server, Reliable)
	//void Server_SetLevelToOpen(ULevelData* LevelData);

	// Timer
	UFUNCTION(BlueprintCallable)
	int32 GetTimeFromTimerEnum();

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 PreGameTimerLength;
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	int32 PostGameTimerLength;

	UPROPERTY(Replicated)
	int32 CurrentTimerTime;
	UPROPERTY(Replicated)
	TEnumAsByte<ETimerEnum> CurrentTimerEnum;

	// Levels
	/*UFUNCTION(BlueprintCallable)
	void ReturnToLobby();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnGameEnd();

	UPROPERTY(Replicated)
	int32 PlayerCount;
	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> PlayersConnected;
	UPROPERTY(Replicated)
	int32 NumOfPlayersReady;
	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> PlayersReady;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* LobbyLevelData;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* WinLevelData;
	UPROPERTY(Replicated)
	FString LevelToOpen;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<ULevelData*> Levels;*/

	UPROPERTY(EditDefaultsOnly, Category = "Nugget")
	UItemData* NuggetItem;

	class AMainGameMode* MainGameMode;
	UGarrysGame_GameInstance* GameInstance;

public:

	// Levels
	//UFUNCTION(BlueprintCallable)
	//void SetLevelToOpen(ULevelData* LevelData);

	//UFUNCTION(BlueprintCallable)
	//void OpenRandomLevel();

	// Players
	//UFUNCTION(BlueprintCallable)
	//void OnPlayerLogin(AController* PlayerController);

	//UFUNCTION(BlueprintCallable)
	//void OnPlayerLogout(AController* PlayerController);


	//UFUNCTION(BlueprintCallable)
	//int32 GetNumOfAlivePlayers();

	//UFUNCTION(BlueprintCallable)
	//int32 GetNumOfConnectedPlayers() const { return PlayerCount; }

	//UFUNCTION(BlueprintCallable)
	//TArray<APlayerCharacter*> GetPlayersConnected();

	//// Playes Ready
	//UFUNCTION(BlueprintCallable)
	//int32 GetNumOfPlayersReady() const { return NumOfPlayersReady; }

	//UFUNCTION(BlueprintCallable)
	//TArray<APlayerCharacter*> GetPlayersReady() const { return PlayersReady; }

	//UFUNCTION(BlueprintCallable)
	//void AddPlayerReady(APlayerCharacter* Player);

	//UFUNCTION(BlueprintCallable)
	//bool IsAllPlayersReady() { return NumOfPlayersReady >= GetNumOfAlivePlayers(); }

	// Timer
	UFUNCTION(BlueprintCallable)
	int32 SubtractTime() { CurrentTimerTime--; return CurrentTimerTime; };

	UFUNCTION(BlueprintCallable)
	void SetTimerTime(int32 Time) { CurrentTimerTime = Time; }

	UFUNCTION(BlueprintCallable)
	int32 GetTimerTime() const { return CurrentTimerTime; }

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<ETimerEnum> GetTimerEnum() const { return CurrentTimerEnum; }

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<ETimerEnum> MoveToNextTimerType();

	UFUNCTION(BlueprintCallable)
	void SetTimerType(TEnumAsByte<ETimerEnum> TimerType) { CurrentTimerEnum = TimerType; }

	// Levels
	//UFUNCTION(BlueprintCallable)
	//ULevelData* GetLobbyData() const { return LobbyLevelData; }

	//UFUNCTION(BlueprintCallable)
	//TArray<ULevelData*> GetLevels() const { return Levels; }
};
