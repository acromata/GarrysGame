#include "../Lobby/McNuggetBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "../Player/PlayerCharacter.h"
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
	
	// Overlap Bindings
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AMcNuggetBox::OnOverlapBegin);
}

void AMcNuggetBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (IsValid(Player) && IsValid(Player->GetEquippedItem()))
	{
		if (Player->GetEquippedItem()->GetItemName() == "Nugget")
		{
			Player->SetEquippedItem(nullptr);
			NuggetsInserted++;

			AGarrysGameGameState* GameState = Cast<AGarrysGameGameState>(GetWorld()->GetGameState());
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Player Count: %f Nugget Count: %f"), GameState->GetNumOfAlivePlayers(), NuggetsInserted));
			if (NuggetsInserted == GameState->GetNumOfAlivePlayers())
			{
				int32 RandNum = FMath::RandRange(0, LevelNames.Num() - 1);
				if (LevelNames.IsValidIndex(RandNum))
				{
					GameState->SetLevelToOpen(LevelNames[RandNum]);
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Called to open level");
				}
				
			}
		}
	}
}

