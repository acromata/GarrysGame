#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../DataAssets/LevelData.h"
#include "../Player/PlayerCharacter.h"
#include "../GameInstance/GarrysGame_GameInstance.h"
#include "MainGameMode.generated.h"


UCLASS()
class GARRYSGAME_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()


protected:

	// Player
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();
	UFUNCTION(BlueprintCallable)
	void OnGameEnd();

	int32 NumOfConnectedPlayers;
	TArray<APlayerCharacter*> ConnectedPlayers;
	int32 NumOfPlayersReady;
	TArray<APlayerCharacter*> PlayersReady;

	// Level
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* LobbyLevelData;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* WinLevelData;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TArray<ULevelData*> Levels;
	UPROPERTY(BlueprintReadOnly)
	FString LevelToOpen;

	// Game instance
	UGarrysGame_GameInstance* GameInstance;

public:

	// Levels
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OpenLevel(const FString& Level);

	UFUNCTION(BlueprintCallable)
	void SetLevelToOpen(ULevelData* LevelData);

	UFUNCTION(BlueprintCallable)
	void OpenRandomLevel();

	UFUNCTION(BlueprintCallable)
	ULevelData* GetLobbyData() const { return LobbyLevelData; }

	UFUNCTION(BlueprintCallable)
	TArray<ULevelData*> GetLevels() const { return Levels; }

	// Players
	UFUNCTION(BlueprintCallable)
	void OnPlayerDeath();

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfAlivePlayers();

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfConnectedPlayers() const { return NumOfConnectedPlayers; }

	UFUNCTION(BlueprintCallable)
	TArray<APlayerCharacter*> GetConnectedPlayers() { return ConnectedPlayers; }

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
};
