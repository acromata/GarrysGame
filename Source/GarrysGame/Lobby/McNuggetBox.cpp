#include "../Lobby/McNuggetBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../DataAssets/LevelData.h"
#include "../GameState/GarrysGameGameState.h"

// Sets default values
AMcNuggetBox::AMcNuggetBox()
{

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("McNuggets");
	Mesh->SetupAttachment(RootComponent);

	Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	Collider->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AMcNuggetBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMcNuggetBox::Interact(APlayerCharacter* Player)
{
	if (IsValid(Player->GetEquippedItem()) && Player->GetEquippedItem()->GetItemName() == "Nugget")
	{
		Player->SetEquippedItem(nullptr);
		NuggetsInserted++;

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Nugget Inserted"));

		AGarrysGameGameState* GameState = Cast<AGarrysGameGameState>(GetWorld()->GetGameState());
		if (NuggetsInserted == GameState->GetNumOfAlivePlayers())
		{
			int32 RandNum = FMath::RandRange(0, GameState->GetLevels().Num() - 1);
			if (GameState->GetLevels().IsValidIndex(RandNum))
			{
				GameState->SetLevelToOpen(GameState->GetLevels()[RandNum]);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Invalid level, Returning to lobby...");
				GameState->SetLevelToOpen(GameState->GetLobbyData());
			}
		}
	}
}

