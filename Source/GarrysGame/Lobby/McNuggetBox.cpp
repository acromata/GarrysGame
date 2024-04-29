#include "../Lobby/McNuggetBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../DataAssets/LevelData.h"
#include "../GameState/GarrysGameGameState.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMcNuggetBox::AMcNuggetBox()
{

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("McNuggets");
	Mesh->SetupAttachment(RootComponent);

	Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	Collider->SetupAttachment(Mesh);
}


void AMcNuggetBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMcNuggetBox, NuggetsInserted);
}

void AMcNuggetBox::Interact(APlayerCharacter* Player)
{
	if (IsValid(Player->GetEquippedItem()) && Player->GetEquippedItem()->GetItemName() == "Nugget")
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Added nugget");

		Player->SetEquippedItem(nullptr);
		NuggetsInserted++;

		AGarrysGameGameState* GameState = Cast<AGarrysGameGameState>(GetWorld()->GetGameState());
		if (IsValid(GameState))
		{
			if (NuggetsInserted == GameState->GetNumOfAlivePlayers())
			{
				GameState->OpenRandomLevel();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Nugget Inserted"));
			}
		}
	}
}

