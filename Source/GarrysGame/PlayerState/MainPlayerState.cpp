#include "../PlayerState/MainPlayerState.h"
#include "../Player/PlayerCharacter.h"

void AMainPlayerState::SetPlayerUsername(FString NewName)
{
	PlayerUsername = NewName;

	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningController()->GetPawn());
	if (IsValid(Player))
	{
		Player->SetPlayerName(NewName);
	}
}
