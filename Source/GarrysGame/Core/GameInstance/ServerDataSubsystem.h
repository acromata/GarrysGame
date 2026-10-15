

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ServerDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GARRYSGAME_API UServerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Server only subsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:

	UPROPERTY(BlueprintReadWrite)
	class ULevelData* CurrentLevelData;

	// Player data
	UPROPERTY(BlueprintReadWrite)
	TSet<FUniqueNetIdRepl> PlayerUUIDsAliveInGame;
	
	
};
