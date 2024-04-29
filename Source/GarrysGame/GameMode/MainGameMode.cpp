#include "../GameMode/MainGameMode.h"
#include "../GameState/GarrysGameGameState.h"

void AMainGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	MyGameState = Cast<AGarrysGameGameState>(GameState);
}

void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
	MyGameState->OnPlayerLogin(NewPlayer);
}

void AMainGameMode::Logout(AController* Exiting)
{
	MyGameState->OnPlayerLogout(Exiting);
}

void AMainGameMode::OpenNewLevel(FString LevelToOpen)
{
	UE_LOG(LogTemp, Warning, TEXT("Open Level Called"));
	GetWorld()->ServerTravel(LevelToOpen);
}
