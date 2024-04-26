// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../DataAssets/ItemData.h"
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

	// Movement
	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);
	UFUNCTION(Server, Reliable)
	void OnJump();
	UFUNCTION(NetMulticast, Reliable)
	void HandleJump();

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump")
	float JumpForceWhileSliding;
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bAllowInput;

	bool bCanMove;

	// Sprinting
	UFUNCTION(Server, Unreliable)
	void StartSprint();
	UFUNCTION(Server, Unreliable)
	void EndSprint();
	UFUNCTION(NetMulticast, Unreliable)
	void HandleSprint();

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
	void StartCrouch();
	UFUNCTION(Server, Unreliable)
	void EndCrouch();
	UFUNCTION(NetMulticast, Unreliable)
	void HandleCrouch();

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsCrouching;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsSliding;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float SlideForce;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Slide")
	float CounterSlideForce;
	UPROPERTY(Replicated)
	float CurrentSlideForce;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector SlideDirection;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsAwaitingSlideJump;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanSlideJump;

	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	USoundBase* SlideSound;

	// Hitting
	UFUNCTION(Server, Reliable)
	void ServerHit();
	UFUNCTION(NetMulticast, Reliable)
	void HandleHit();
	UFUNCTION(Server, Reliable)
	void AllowHitting();
	UFUNCTION(BlueprintImplementableEvent)
	void Die();

	void TickKnockback();
	void EndKnockback();

	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDistance;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitDelay;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitForce;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	float HitKnockbackTime;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Hitting")
	UAnimMontage* HitAnimation;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector HitDirection;
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanHit;

	// For Knockback
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bWasHit;

	float KnockbackForce;

	// Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 MaxHealth;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	int32 CurrentHealth;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bIsDead;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Health")
	bool bCanTakeDamage;

	// Items
	UFUNCTION(NetMulticast, Reliable)
	void SetEquippedItem_Multicast(UItemData* Item);

	UPROPERTY(Replicated, BlueprintReadWrite, EditDefaultsOnly)
	UItemData* ItemEquipped;

	// Interacting
	void Interact();

	UPROPERTY(EditDefaultsOnly, Category = "Interacting")
	float InteractRange;

	// Minigames
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsSafeFromStatue;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float PlayerScore;

	UPROPERTY(EditDefaultsOnly, Category = "TagMinigame")
	UItemData* StickTagItem;

	// Player Name
	UPROPERTY(Replicated)
	FString PlayerName;

public:

	// Health
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SubtractHealth(int32 Health);

	// Equipped Items
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetEquippedItem(UItemData* Item);

	UFUNCTION(BlueprintCallable, Category = "Items")
	UItemData* GetEquippedItem() const { return ItemEquipped; }

	// Player Score
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetPlayerScore(float NewScore);

	// Player Score
	UFUNCTION(BlueprintCallable, Category = "Minigames")
	float GetPlayerScore() const { return PlayerScore; }

	// Check is dead
	bool GetIsDead() const { return bIsDead; }

	// Add Knockback
	void StartKnockback(FVector NewHitDirection, float NewKnockbackForce);

	// Input
	UFUNCTION(BlueprintCallable)
	void EnablePlayerInput() { bAllowInput = true; }

	// Player Name
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() const { return PlayerName; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(FString Name) { PlayerName = Name; }
};
