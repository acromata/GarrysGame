#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/InteractableInterface.h"
#include "../DataAssets/LevelData.h"
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

	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(NetMulticast, Reliable)
	void PlaySFXForClients(float Pitch = 1.f);

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 NuggetsInserted;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	ULevelData* LevelToOpenOverride;
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* NuggetInsertedSFX;
	UPROPERTY(EditDefaultsOnly, Category = "Nugget Item")
	UItemData* NuggetItem;

	class AGarrysGameGameState* MainGameState;

public:

	virtual void Interact(APlayerCharacter* Player) override;
};
