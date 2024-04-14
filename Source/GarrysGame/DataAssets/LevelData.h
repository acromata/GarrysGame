#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../DataAssets/MinigameData.h"
#include "LevelData.generated.h"

UENUM(BlueprintType)
enum ELevelMinigame
{
	Lobby,
	Statue,
	GlassDrop,
	KOTH,
	StickTag
};

UCLASS()
class GARRYSGAME_API ULevelData : public UDataAsset
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly)
	FString LevelName;
	UPROPERTY(EditDefaultsOnly)
	FString LevelDisplayName;
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ELevelMinigame> LevelMinigame;
	UPROPERTY(EditDefaultsOnly)
	UMinigameData* MinigameData;

public:

	UFUNCTION(BlueprintCallable)
	FString GetLevelName() const { return LevelName; }
	UFUNCTION(BlueprintCallable)
	FString GetDisplayName() const { return LevelDisplayName; }
	//UFUNCTION(BlueprintCallable)
	//TEnumAsByte<ELevelMinigame> GetLevelMinigame() const { return LevelMinigame; }
	UFUNCTION(BlueprintCallable)
	UMinigameData* GetMinigameData() const { return MinigameData; }

};
