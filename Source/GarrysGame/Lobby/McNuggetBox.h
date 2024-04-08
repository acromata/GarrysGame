#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "McNuggetBox.generated.h"

UCLASS()
class GARRYSGAME_API AMcNuggetBox : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
	
public:	
	// Sets default values for this actor's properties
	AMcNuggetBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(BlueprintReadOnly)
	int32 NuggetsInserted;
};
