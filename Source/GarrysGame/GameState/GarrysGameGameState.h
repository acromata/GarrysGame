#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../Player/PlayerCharacter.h"
#include "../DataAssets/ItemData.h"
#include "GarrysGameGameState.generated.h"

UCLASS()
class GARRYSGAME_API AGarrysGameGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	// Network Updates
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnPlayerLogin(AController* PlayerController);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnPlayerLogout(AController* PlayerController);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnPlayerDeath();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ReturnToLobby();

	UFUNCTION(BlueprintCallable)
	void OpenLevel();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OnGameEnd();

	UPROPERTY(Replicated)
	int32 PlayerCount;
	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> PlayersConnected;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FString LobbyMapName;
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FString WinMapName;
	UPROPERTY(Replicated)
	FString LevelToOpen;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Maps")
	TArray<FString> LevelNames;

	UPROPERTY(EditDefaultsOnly, Category = "Nugget")
	UItemData* NuggetItem;

public:

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetLevelToOpen(const FString& LevelName);

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfAlivePlayers() const;

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfConnectedPlayers() const { return PlayerCount; }

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetLevelNames() const { return LevelNames;  }
};
