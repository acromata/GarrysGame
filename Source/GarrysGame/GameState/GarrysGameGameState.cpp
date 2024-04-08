#include "../GameState/GarrysGameGameState.h"
#include "Net/UnrealNetwork.h"

void AGarrysGameGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGarrysGameGameState, PlayerCount);
	DOREPLIFETIME(AGarrysGameGameState, PlayersConnected);
	DOREPLIFETIME(AGarrysGameGameState, LevelToOpen);
}

void AGarrysGameGameState::OnPlayerLogin_Implementation(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Add(Player);
		PlayerCount++;

		// Check if in lobby
		FString CurrentLevelName = GetWorld()->GetMapName();
		CurrentLevelName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
		if (CurrentLevelName == LobbyMapName)
		{
			Player->SetEquippedItem(NuggetItem);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Nuggeted");
		}
		else
		{
			//Player->SubtractHealth(420.f);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, ("In Map: %s", GetWorld()->GetMapName()));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("Player Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerLogout_Implementation(AController* PlayerController)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetPawn());
	if (IsValid(Player))
	{
		PlayersConnected.Remove(Player);
		PlayerCount--;

		GEngine->AddOnScreenDebugMessage(-1, 0.5, FColor::Red, FString::Printf(TEXT("Player Count: %f"), PlayerCount));
	}
}

void AGarrysGameGameState::OnPlayerDeath_Implementation()
{
	if (GetNumOfAlivePlayers() <= 0)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGarrysGameGameState::ReturnToLobby, 2.f);
	}
}

int32 AGarrysGameGameState::GetNumOfAlivePlayers() const
{
	int32 AlivePlayerCount = PlayerCount;
	
	for (APlayerCharacter* Player : PlayersConnected)
	{
		if (Player->GetIsDead())
		{
			AlivePlayerCount--;
		}
	}

	return AlivePlayerCount;
}

void AGarrysGameGameState::ReturnToLobby_Implementation()
{
	SetLevelToOpen("Lobby");
}

void AGarrysGameGameState::SetLevelToOpen_Implementation(const FString& LevelName)
{
	LevelToOpen = LevelName;
	OpenLevel();

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Opening level %s", LevelToOpen));
}

void AGarrysGameGameState::OpenLevel()
{
	GetWorld()->ServerTravel(LevelToOpen);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Server travel called"));
}

void AGarrysGameGameState::OnGameEnd_Implementation()
{
	if (GetNumOfAlivePlayers() < 1)
	{
		int32 RandNum = FMath::RandRange(0, LevelNames.Num() - 1);
		if (LevelNames.IsValidIndex(RandNum))
		{
			SetLevelToOpen(LevelNames[RandNum]);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Invalid level");
			SetLevelToOpen("Lobby");
		}
	}
	else if(GetNumOfAlivePlayers() == 1)
	{
		SetLevelToOpen("WinMapName");
	}
	else
	{
		SetLevelToOpen(LobbyMapName);
	}
}
