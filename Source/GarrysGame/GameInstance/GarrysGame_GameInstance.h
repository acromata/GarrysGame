#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../DataAssets/LevelData.h"
#include "../Player/PlayerCharacter.h"
#include "GarrysGame_GameInstance.generated.h"

UCLASS()
class GARRYSGAME_API UGarrysGame_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated)
	ULevelData* CurrentLevelData;

public:

	UFUNCTION(BlueprintCallable)
	void SetCurrentLevel(ULevelData* Level) { CurrentLevelData = Level; }

	UFUNCTION(BlueprintCallable)
	ULevelData* GetCurrentLevel() const { return CurrentLevelData; }

};
