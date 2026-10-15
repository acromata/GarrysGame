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
	UPROPERTY(BlueprintReadOnly)
	class AMainGameMode* MainGameMode;

	// Movement
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	void StartJump();
	void EndJump();
	UFUNCTION(Server, Reliable)
	void Server_StartSlideJump();
	UFUNCTION(Server, Reliable)
	void Server_EndSlideJump();

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
	float CrouchSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float WalkSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float RunSpeed = 800.f;

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

	// Slide
	void StartSlide();
	void EndSlide();
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HandleSlide();

	UFUNCTION()
	void OnRep_IsSliding();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsGroundedDuringSlide();

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsCrouching;
	UPROPERTY(ReplicatedUsing = OnRep_IsSliding, BlueprintReadOnly)
	bool bIsSliding;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsSlideJumping;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float SlideForce = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float CounterSlideForce = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float SlideGroundedRadiusCheck = 50.f;
	UPROPERTY(Replicated, BlueprintReadOnly)
	float CurrentSlideForce;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float JumpForceWhileSliding = 420.f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector SlideDirection;

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
	FVector HitBoxSize = FVector(20.f);
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDelay = .2f;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitForce = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	UAnimMontage* HitAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDamagePercentage = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDamage = 20.f;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector HitDirection;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector HitLocation;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanHit = true;

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
	void Server_Die(bool bShowRagdoll);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 MaxHealth = 100.f;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	int32 CurrentHealth;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bIsDead;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bCanTakeDamage = true;
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
	float InteractRange = 500.f;

	// Doll minigame
	UFUNCTION(BlueprintCallable)
	void SetLastSeenPosition() { DollLastSeenTransform = GetActorTransform(); }
	UFUNCTION(BlueprintCallable)
	bool IsMovedFromLastSeenPosition();
	UPROPERTY(EditAnywhere, Category = "Minigames|Doll")
	float DollMoveErrorMargin = 50;
	float DollLookErrorMargin = 10;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FTransform DollLastSeenTransform;

	// Score
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerScore(float NewScore);
	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerScore;

	// Tag minigame
	UPROPERTY(EditDefaultsOnly, Category = "Minigames|Tag")
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
	void Die(bool bShowRagdoll = true);

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
