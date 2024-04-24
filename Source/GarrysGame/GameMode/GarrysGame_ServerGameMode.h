// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GarrysGame_ServerGameMode.generated.h"

struct FProcessParameters;

DECLARE_LOG_CATEGORY_EXTERN(GameServerLog, Log, All);

UCLASS(minimalapi)
class GARRYSGAME_API AGarrysGame_ServerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    AGarrysGame_ServerGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void InitGameLift();

    TSharedPtr<FProcessParameters> ProcessParameters;
};