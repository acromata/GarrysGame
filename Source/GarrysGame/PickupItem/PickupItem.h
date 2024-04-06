#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../DataAssets/ItemData.h"
#include "PickupItem.generated.h"

UCLASS()
class GARRYSGAME_API APickupItem : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemData* ItemData;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};
