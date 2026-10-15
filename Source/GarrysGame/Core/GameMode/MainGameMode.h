#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

class UServerDataSubsystem;
class APlayerCharacter;
class UItemData;

UCLASS()
class GARRYSGAME_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;

	virtual void Logout(AController* Exiting) override;

	// Player
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();
	UFUNCTION(BlueprintCallable)
	void OnGameEnd();

	// Level
	void SetCurrentLevel(ULevelData* Data);

	UPROPERTY(BlueprintReadOnly)
	FString LevelToOpen;

	// Game State
	UPROPERTY(BlueprintReadWrite)
	class AGarrysGameGameState* MainGameState;

	// Heartbeats
	void CheckForMissedHeartbeats();

	TMap<APlayerCharacter*, int32> MissedHeartbeatsMap;

	UPROPERTY(EditDefaultsOnly, Category = "Heartbeat")
	FName HeartbeatDisconnectMapName;

	UPROPERTY(EditAnywhere, Category = "Lobby")
	UItemData* LobbyNuggetItem;

public:

	// Levels
	UFUNCTION(BlueprintCallable)
	void SetLevelToOpen(ULevelData* LevelData);

	UFUNCTION(BlueprintCallable)
	void OpenRandomLevel();

	void OnPlayerStart(APlayerCharacter* Player);

	// Players
	UFUNCTION(BlueprintCallable)
	void OnPlayerDeath(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable)
	TArray<APlayerCharacter*> GetConnectedPlayers();

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfConnectedPlayers() { return GetConnectedPlayers().Num(); }

	UFUNCTION(BlueprintCallable)
	TArray<APlayerCharacter*> GetAlivePlayers();

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfAlivePlayers() { return GetAlivePlayers().Num(); }

	UFUNCTION(BlueprintCallable)
	APlayerCharacter* GiveRandomPlayerItem(UItemData* Item);

	// Heartbeat
	void ReceiveHeartbeat(APlayerCharacter* Player);

	// Player with stick
	UPROPERTY(BlueprintReadWrite)
	APlayerCharacter* PlayerTagged;

	UPROPERTY(BlueprintReadOnly)
	UServerDataSubsystem* ServerDataSubsystem;
};
