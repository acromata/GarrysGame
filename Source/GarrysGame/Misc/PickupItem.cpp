#include "../Misc/PickupItem.h"
#include "../Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Replication
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMesh>("Mesh");
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Mesh
	Mesh = ItemData->GetItemMesh();
}

// Called every frame
void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (IsValid(Player))
	{
		Player->SetEquippedItem(ItemData);
		Destroy();
	}
}

