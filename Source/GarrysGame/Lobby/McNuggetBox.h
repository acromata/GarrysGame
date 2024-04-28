#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/InteractableInterface.h"
#include "McNuggetBox.generated.h"

UCLASS()
class GARRYSGAME_API AMcNuggetBox : public AActor, public IInteractableInterface
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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 NuggetsInserted;

public:

	virtual void Interact(APlayerCharacter* Player) override;
};
