#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MinigameData.generated.h"

/**
 * 
 */
UCLASS()
class GARRYSGAME_API UMinigameData : public UDataAsset
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly)
	FString MinigameName;
	UPROPERTY(EditDefaultsOnly)
	FString MinigameDescription;
	UPROPERTY(EditDefaultsOnly)
	int32 MinigameTime;
	UPROPERTY(EditDefaultsOnly)
	int32 MinimumPlayerCount = 1;

public:

	UFUNCTION(BlueprintCallable)
	FString GetMinigameName() const { return MinigameName; }
	UFUNCTION(BlueprintCallable)
	FString GetMinigameDescription() const { return MinigameDescription; }
	UFUNCTION(BlueprintCallable)
	int32 GetMinigameTime() const { return MinigameTime; }
	UFUNCTION(BlueprintCallable)
	int32 GetMinimumPlayerCount() const { return MinimumPlayerCount; }
};
