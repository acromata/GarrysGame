#include "../Player/PlayerCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "../Interfaces/InteractableInterface.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	bUseControllerRotationYaw = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Item Mesh
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("Item");
	ItemMesh->SetupAttachment(GetMesh(), "ItemSocket");

	// Movement
	JumpForceWhileSliding = 420.f;

	// Speeds
	CrouchSpeed = 400.f;
	WalkSpeed = 400.f;
	RunSpeed = 800.f;

	// Slide
	SlideForce = 1000.f;
	CounterSlideForce = 1.f;
	SlideJumpDelay = 10.f;

	// Hitting
	bCanHit = true;
	HitDistance = 100.f;
	HitDelay = 0.5f;
	HitForce = 1000.f;

	// Health
	MaxHealth = 100.f;
	bCanTakeDamage = true;

	// Interactable
	InteractRange = 500.f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Movement
	bCanMove = true;
	bAllowInput = false;

	// Speeds
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	CurrentSlideForce = SlideForce;

	// Health
	CurrentHealth = MaxHealth;
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickKnockback();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add input mapping context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get local player subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add input context
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	// Bind Inputs
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnJump);

		Input->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartSprint);
		Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndSprint);

		Input->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartCrouch);
		Input->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndCrouch);

		Input->BindAction(HitAction, ETriggerEvent::Triggered, this, &APlayerCharacter::ServerHit);

		Input->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Movement
	DOREPLIFETIME(APlayerCharacter, bAllowInput);

	// Health
	DOREPLIFETIME(APlayerCharacter, CurrentHealth);
	DOREPLIFETIME(APlayerCharacter, bIsDead);
	DOREPLIFETIME(APlayerCharacter, bCanTakeDamage);

	// Running
	DOREPLIFETIME(APlayerCharacter, bIsRunning);

	// Crouch
	DOREPLIFETIME(APlayerCharacter, bIsCrouching);

	// Sliding
	DOREPLIFETIME(APlayerCharacter, CurrentSlideForce);
	DOREPLIFETIME(APlayerCharacter, SlideDirection);
	DOREPLIFETIME(APlayerCharacter, bIsSliding);
	DOREPLIFETIME(APlayerCharacter, bIsAwaitingSlideJump);
	DOREPLIFETIME(APlayerCharacter, CurrentSlideJumpDelay);

	// Hitting
	DOREPLIFETIME(APlayerCharacter, HitDirection);
	DOREPLIFETIME(APlayerCharacter, bCanHit);
	DOREPLIFETIME(APlayerCharacter, bWasHit);

	// Items
	DOREPLIFETIME(APlayerCharacter, ItemEquipped);

	// Minigames
	DOREPLIFETIME(APlayerCharacter, bIsSafeFromStatue);
	DOREPLIFETIME(APlayerCharacter, PlayerScore);


}

#pragma region Movement

void APlayerCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller) && bCanMove && bAllowInput)
	{
		// Get forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement input
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void APlayerCharacter::OnJump_Implementation()
{
	if (bAllowInput)
	{
		if (bIsSliding)
		{
			bIsAwaitingSlideJump = true;
		}
		else
		{
			HandleJump();
		}
	}
}

void APlayerCharacter::HandleJump_Implementation()
{
	ACharacter::Jump();
}

#pragma endregion

#pragma region Sprint

void APlayerCharacter::StartSprint_Implementation()
{
	if (GetVelocity().Size() >= 0.5)
	{
		bIsRunning = true;
	}
	else
	{
		bIsRunning = false;
	}

	HandleSprint();
}

void APlayerCharacter::EndSprint_Implementation()
{
	bIsRunning = false;
	HandleSprint();
}

void APlayerCharacter::HandleSprint_Implementation()
{
	if (bIsRunning && !bIsCrouched)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

#pragma endregion

#pragma region Crouch

void APlayerCharacter::StartCrouch_Implementation()
{
	if (GetCharacterMovement()->CanCrouchInCurrentState() && bAllowInput)
	{
		bIsCrouching = true;

		if ((bIsRunning || GetVelocity().Size() > WalkSpeed + 50.f) && CurrentSlideForce > CrouchSpeed && // Check if fast enough
		(GetCharacterMovement()->IsMovingOnGround() || bIsSliding)) // Check if grounded
		{
			// Play Sound
			if (!bIsSliding)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SlideSound, GetActorLocation(), GetActorRotation(), 1.5f);
			}

			// Allow Sliding
			bIsSliding = true;

			// Jump delay timer
			CurrentSlideJumpDelay--;

			// Get Slide Direction
			SlideDirection = CurrentSlideForce * GetVelocity().GetUnsafeNormal();
			if (bIsAwaitingSlideJump)
			{
				if (CurrentSlideJumpDelay <= 0)
				{
					SlideDirection.Z = JumpForceWhileSliding;
					CurrentSlideJumpDelay = SlideJumpDelay;
				}
			}
			else
			{
				SlideDirection.Z = 0.f;
			}
		}
		else
		{
			bIsSliding = false;
			CurrentSlideJumpDelay = 0;
		}
	}
	else
	{
		bIsCrouching = false;
		bIsSliding = false;
		CurrentSlideJumpDelay = 0;
	}

	HandleCrouch();
}

void APlayerCharacter::EndCrouch_Implementation()
{
	bIsCrouching = false;
	bIsSliding = false;
	CurrentSlideJumpDelay = 0;
	HandleCrouch();
}

void APlayerCharacter::HandleCrouch_Implementation()
{
	// Crouch
	if (bIsCrouching)
	{
		// Crouch
		Crouch();
	}
	else
	{
		// Uncrouch
		UnCrouch();

		// Reset slide
		CurrentSlideForce = SlideForce;
	}

	// Slide
	if (bIsSliding && bIsCrouched)
	{
		// Add Forward Force
		LaunchCharacter(SlideDirection, true, false);

		// Add Counterforce
		CurrentSlideForce -= CounterSlideForce;

		// Disable movement when turning camera
		bUseControllerRotationYaw = false;

		// Disable jump force
		bIsAwaitingSlideJump = false;
	}
	else
	{
		// Unslide
		bCanMove = true;
		bUseControllerRotationYaw = true;
		bIsAwaitingSlideJump = false;
	}
}

#pragma endregion

#pragma region Hitting

void APlayerCharacter::ServerHit_Implementation()
{
	if (bCanHit && bAllowInput)
	{
		HandleHit();
	}
}

void APlayerCharacter::HandleHit_Implementation()
{
	// Hit Delay
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &APlayerCharacter::AllowHitting, HitDelay);
	bCanHit = false;

	// Play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation(), GetActorRotation(), 1.5f);

	// Animation
	UAnimInstance* AnimationInstance = GetMesh()->GetAnimInstance();
	if (IsValid(HitAnimation))
	{
		AnimationInstance->Montage_Play(HitAnimation);
	}


	// Line Trace
	FVector StartLocation = Camera->GetComponentLocation();
	FVector EndLocation = StartLocation + (Camera->GetComponentRotation().Vector() * HitDistance);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// The line trace
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	// If hit
	if (bIsHit)
	{
		APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(HitResult.GetActor());
		if (IsValid(HitPlayer))
		{
			// Get Direction
			FVector NewHitDirection = (HitPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

			// Launch
			if (IsValid(GetEquippedItem()) && GetEquippedItem()->GetItemType() == EItemType::TagItem)
			{
				// If item is tag item, do extra knockback
				HitPlayer->StartKnockback(NewHitDirection, HitForce * GetEquippedItem()->GetItemValue());
				HitPlayer->SetEquippedItem(StickTagItem);
				SetEquippedItem(nullptr);
			}
			else
			{
				HitPlayer->StartKnockback(NewHitDirection, HitForce);
			}
		}
	}

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::White, false, 1, 0, 1);

}

void APlayerCharacter::AllowHitting_Implementation()
{
	bCanHit = true;
}

void APlayerCharacter::StartKnockback(FVector NewHitDirection, float NewKnockbackForce)
{
	bWasHit = true;
	HitDirection = NewHitDirection;
	KnockbackForce = NewKnockbackForce;

	FTimerHandle EndKnockbackTimer;
	GetWorld()->GetTimerManager().SetTimer(EndKnockbackTimer, this, &APlayerCharacter::EndKnockback, HitKnockbackTime);
}

void APlayerCharacter::TickKnockback()
{
	if (bWasHit == true)
	{
		LaunchCharacter(HitDirection * KnockbackForce, true, false);
	}
}

void APlayerCharacter::EndKnockback()
{
	bWasHit = false;
}

#pragma endregion

#pragma region Health

void APlayerCharacter::SubtractHealth_Implementation(int32 Health)
{
	if (bCanTakeDamage)
	{
		CurrentHealth -= Health;
	}
	
	if (CurrentHealth <= 0)
	{
		// Die
		Die();
	}
}

#pragma endregion

#pragma region Item Equip

void APlayerCharacter::SetEquippedItem_Implementation(UItemData* Item)
{
	SetEquippedItem_Multicast(Item);
}

void APlayerCharacter::SetEquippedItem_Multicast_Implementation(UItemData* Item)
{
	if (IsValid(Item))
	{
		ItemEquipped = Item;
		ItemMesh->SetStaticMesh(ItemEquipped->GetItemMesh());
	}
	else
	{
		ItemEquipped = nullptr;
		ItemMesh->SetStaticMesh(nullptr);
	}
	
}

#pragma endregion

#pragma region Interact

void APlayerCharacter::Interact()
{
	TArray<FHitResult> HitResult;

	bool bHit = GetWorld()->SweepMultiByChannel(HitResult, GetActorLocation(), GetActorLocation(),
		FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(InteractRange));

	//DrawDebugSphere(GetWorld(), GetActorLocation(), InteractRange, 20, FColor::Purple, true, 1.f);

	if (bHit)
	{
		for (FHitResult Hit : HitResult)
		{
			IInteractableInterface* Interactable = Cast<IInteractableInterface>(Hit.GetActor());
			if (Interactable)
			{
				Interactable->Interact(this);
			}
		}

	}
}

#pragma endregion

#pragma region Minigames

void APlayerCharacter::SetPlayerScore_Implementation(float NewScore)
{
	PlayerScore = NewScore;
}

#pragma endregion
