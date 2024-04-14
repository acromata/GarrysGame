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
			if (NuggetsInserted == GameState->GetNumOfAlivePlayers())
			{
				int32 RandNum = FMath::RandRange(0, GameState->GetLevels().Num() - 1);
				if (GameState->GetLevels().IsValidIndex(RandNum))
				{
					GameState->SetLevelToOpen(GameState->GetLevels()[RandNum]);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Invalid level");
					GameState->SetLevelToOpen(GameState->GetLobbyData());
				}
				
			}
		}
	}
}

