#include "../PlayerState/MainPlayerState.h"
#include "../Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void AMainPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPlayerState, PlayerUsername);
}

void AMainPlayerState::SetPlayerUsername(FString NewName)
{
	//PlayerUsername = NewName;

	//APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningController()->GetPawn());
	//if (IsValid(Player))
	//{
	//	Player->SetPlayerName(PlayerUsername);
	//}

	SetPlayerName(NewName);
}
