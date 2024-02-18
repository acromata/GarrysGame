// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GARRYSGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

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
	class UInputAction* SlideAction;

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
	void Jump();

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

	UPROPERTY(Replicated)
	bool bIsCrouching;
	UPROPERTY(Replicated)
	bool bIsSliding;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float SlideForce;
	UPROPERTY(Replicated)
	float CurrentSlideForce;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speeds")
	float CounterSlideForce;
	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector SlideDirection;

	// Health
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	int32 MaxHealth;
	UPROPERTY(Replicated)
	int32 CurrentHealth;

};
