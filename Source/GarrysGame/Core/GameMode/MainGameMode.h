#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GarrysGame/Core/GameInstance/GarrysGame_GameInstance.h"
#include "MainGameMode.generated.h"


UCLASS()
class GARRYSGAME_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;

	// Player
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();
	UFUNCTION(BlueprintCallable)
	void OnGameEnd();

	UPROPERTY(BlueprintReadWrite)
	int32 NumOfPlayersReady;
	UPROPERTY(BlueprintReadWrite)
	TArray<APlayerCharacter*> PlayersReady;

	// Level
	UFUNCTION(BlueprintImplementableEvent)
	void OpenLevel(const FString& Level);

	void SetCurrentLevel(ULevelData* Data);

	FString LevelToOpen;

	// Game instance
	UPROPERTY(BlueprintReadWrite)
	UGarrysGame_GameInstance* GameInstance;

	// Game State
	UPROPERTY(BlueprintReadWrite)
	class AGarrysGameGameState* MainGameState;

	// Heartbeats
	void CheckForMissedHeartbeats();

	TMap<APlayerCharacter*, int32> MissedHeartbeatsMap;

	UPROPERTY(EditDefaultsOnly, Category = "Heartbeat")
	FName HeartbeatDisconnectMapName;

public:

	// Levels
	UFUNCTION(BlueprintCallable)
	void SetLevelToOpen(ULevelData* LevelData);

	UFUNCTION(BlueprintCallable)
	void OpenRandomLevel();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CheckIfInLobby(APlayerCharacter* Player);

	// Players
	UFUNCTION(BlueprintCallable)
	void OnPlayerDeath();

	UFUNCTION(BlueprintCallable)
	TArray<APlayerCharacter*> GetConnectedPlayers();

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfConnectedPlayers() { return GetConnectedPlayers().Num(); }

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfAlivePlayers();

	// Players Ready
	UFUNCTION(BlueprintCallable)
	int32 GetNumOfPlayersReady() const { return NumOfPlayersReady; }

	UFUNCTION(BlueprintCallable)
	TArray<APlayerCharacter*> GetPlayersReady() const { return PlayersReady; }

	UFUNCTION(BlueprintCallable)
	void AddPlayerReady(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	bool IsAllPlayersReady() { return NumOfPlayersReady >= GetNumOfAlivePlayers(); }

	UFUNCTION(BlueprintCallable)
	void GiveRandomPlayerItem(UItemData* Item);

	// Heartbeat
	void ReceiveHeartbeat(APlayerCharacter* Player);
};
