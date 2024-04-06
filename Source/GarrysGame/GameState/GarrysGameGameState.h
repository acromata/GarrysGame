#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../Player/PlayerCharacter.h"
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

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void OpenLevel();

	UPROPERTY(Replicated)
	int32 PlayerCount;
	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> PlayersConnected;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FString LobbyMapName;
	UPROPERTY(Replicated)
	FString LevelToOpen;

public:

	UFUNCTION(BlueprintCallable)
	FString SetLevelToOpen(FString LevelName);

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfAlivePlayers() const;

	UFUNCTION(BlueprintCallable)
	int32 GetNumOfConnectedPlayers() const { return PlayerCount; }
};
