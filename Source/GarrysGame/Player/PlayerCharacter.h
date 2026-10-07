// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../DataAssets/ItemData.h"
#include "../PlayerState/MainPlayerState.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GARRYSGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemMesh;

protected:

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* SprintAction;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* HitAction;

	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* InteractAction;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Network Updates
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	// Gay mode
	UPROPERTY(Replicated, BlueprintReadOnly)
	class AMainGameMode* MainGameMode;

	// Movement
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void OnJump();
	void HandleJump();

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bAllowInput;
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bCanMove;

	// Sprinting
	UFUNCTION(Server, Unreliable)
	void Server_StartSprint();
	void StartSprint();
	UFUNCTION(Server, Unreliable)
	void Server_EndSprint();
	void EndSprint();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HandleSprint();

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float CrouchSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float RunSpeed;

	UPROPERTY(Replicated)
	bool bIsRunning;

	// Crouching
	UFUNCTION(Server, Unreliable)
	void Server_StartCrouch();
	void StartCrouch();
	UFUNCTION(Server, Unreliable)
	void Server_EndCrouch();
	void EndCrouch();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HandleCrouch();

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsCrouching;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsSliding;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bhasPlayedSlideSound;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float SlideForce;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float CounterSlideForce;
	UPROPERTY(Replicated)
	float CurrentSlideForce;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float JumpForceWhileSliding;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector SlideDirection;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsAwaitingSlideJump;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanSlideJump;

	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	USoundBase* SlideSound;

	// Hitting
	void Hit();
	void AllowHitting() { bCanHit = true; };

	UFUNCTION(Server, Reliable)
	void Server_Hit();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleHit();

	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitForce;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	UAnimMontage* HitAnimation;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector HitDirection;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanHit;

	// Knockback
	void TickKnockback(float DeltaTime);
	UFUNCTION(Server, Reliable)
	void Server_Knockback(FVector NewHitDirection, float NewKnockbackForce);
	UPROPERTY(Replicated)
	float KnockbackForce;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitKnockbackTime;
	UPROPERTY(Replicated)
	float CurrentKnockbackTime;
	UPROPERTY(Replicated)
	bool bShouldDealKB;

	// Health
	UFUNCTION(Server, Reliable)
	void Server_SubtractHealth(int32 Health);
	UFUNCTION(Server, Reliable)
	void Server_Die();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 MaxHealth;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	int32 CurrentHealth;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bIsDead;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bCanTakeDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TSubclassOf<APawn> SpectatorPawn;

	// Items
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetEquippedItem(UItemData* Item, APlayerCharacter* ReceivingPlayer = nullptr);
	UFUNCTION(Server, Reliable)
	void Server_SetEquippedItem(UItemData* Item, APlayerCharacter* ReceivingPlayer = nullptr);

	UPROPERTY(Replicated, BlueprintReadWrite, EditDefaultsOnly)
	UItemData* ItemEquipped;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	USoundBase* ItemEquippedSound;

	// Interacting
	void Interact();
	UFUNCTION(Server, Reliable)
	void Server_Interact();

	UPROPERTY(EditDefaultsOnly, Category = "Interacting")
	float InteractRange;

	// Minigames
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsSafeFromStatue;

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerScore(float NewScore);
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerScore;

	UPROPERTY(EditDefaultsOnly, Category = "TagMinigame")
	UItemData* StickTagItem;

	// Heartbeat
	void SendHeartbeatToServer();
	UFUNCTION(Server, Reliable)
	void Server_SendHeartbeatToServer();

	int32 NumOfMissedHeartbeats;

public:

	// Health
	UFUNCTION(BlueprintCallable)
	void SubtractHealth(int32 Health);
	UFUNCTION(BlueprintCallable)
	void Die();

	// Equipped Items
	UFUNCTION(BlueprintCallable)
	void SetEquippedItem(UItemData* Item, APlayerCharacter* ReceivingPlayer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Items")
	UItemData* GetEquippedItem() const { return ItemEquipped; }

	// Player Score
	UFUNCTION(BlueprintCallable)
	void SetPlayerScore(float NewScore);

	// Player Score
	UFUNCTION(BlueprintCallable, Category = "Minigames")
	float GetPlayerScore() const { return PlayerScore; }

	// Check is dead
	bool GetIsDead() const { return bIsDead; }

	// Add Knockback
	void Knockback(FVector NewHitDirection, float NewKnockbackForce);

	// Input
	UFUNCTION(BlueprintCallable)
	void EnablePlayerInput() { bAllowInput = true; }

	// Heartbeats
	int32 GetNumOfMissedHeartbeats() const { return NumOfMissedHeartbeats;  }

	// Winzone
	UPROPERTY(BlueprintReadWrite)
	bool bIsInWinzone;
};
