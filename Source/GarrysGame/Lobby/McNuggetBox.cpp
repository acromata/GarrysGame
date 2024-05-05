#include "../Lobby/McNuggetBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Player/PlayerCharacter.h"
#include "../GameState/GarrysGameGameState.h"
#include "../GameMode/MainGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMcNuggetBox::AMcNuggetBox()
{

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("McNuggets");
	Mesh->SetupAttachment(RootComponent);

	Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	Collider->SetupAttachment(Mesh);

	bReplicates = true;
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

		// Add nugget
		NuggetsInserted++;
		Player->SetEquippedItem(nullptr);

		// Play SFX
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), NuggetInsertedSFX, GetActorLocation(), GetActorRotation(), 1.f);

		AMainGameMode* MainGameMode = GetWorld()->GetAuthGameMode<AMainGameMode>();
		if (IsValid(MainGameMode))
		{
			if (NuggetsInserted >= MainGameMode->GetNumOfAlivePlayers())
			{
				if (IsValid(LevelToOpenOverride))
				{
					MainGameMode->SetLevelToOpen(LevelToOpenOverride);
				}
				else
				{
					MainGameMode->OpenRandomLevel();
				}
				
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Nugget Inserted"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, ("Invalid"));
		}
	}
}

