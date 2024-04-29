#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"


UCLASS()
class GARRYSGAME_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

protected:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	// Game state
	AGarrysGameGameState* MyGameState;

	// Levels
	void OpenNewLevel(FString LevelToOpen)
};
