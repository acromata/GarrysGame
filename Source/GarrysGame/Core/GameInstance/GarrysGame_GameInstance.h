#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GarrysGame/DataAssets/LevelData.h"
#include "GarrysGame/Player/PlayerCharacter.h"
#include "GarrysGame_GameInstance.generated.h"

UCLASS()
class GARRYSGAME_API UGarrysGame_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:

	UPROPERTY(Replicated)
	ULevelData* CurrentLevelData;
	UPROPERTY(Replicated)
	TArray<APlayerCharacter*> PlayersDead;

	UFUNCTION(BlueprintCallable)
	void SetCurrentLevel(ULevelData* Level) { CurrentLevelData = Level; }

	UFUNCTION(BlueprintCallable)
	ULevelData* GetCurrentLevel() const { return CurrentLevelData; }

};
