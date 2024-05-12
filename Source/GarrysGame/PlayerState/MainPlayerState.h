#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"


UCLASS()
class GARRYSGAME_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

	// Network Updates
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	FString PlayerUsername;

public:

	// Username
	UFUNCTION(BlueprintCallable)
	FString GetPlayerUsername() const { return PlayerUsername; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerUsername(FString NewName);
};
