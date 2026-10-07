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
#include "GarrysGame/Core/GameInstance/GarrysGame_GameInstance.h"
#include "GarrysGame/Core/GameMode/MainGameMode.h"
#include "Components/CapsuleComponent.h"

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
	bhasPlayedSlideSound = false;

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

	if (HasAuthority())
	{
		// Movement
		bCanMove = true;
		bAllowInput = false;

		// Speeds
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
		CurrentSlideForce = SlideForce;

		// Health
		CurrentHealth = MaxHealth;

		// Send a heartbeat to server
		FTimerHandle HeartbeatTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &APlayerCharacter::SendHeartbeatToServer, 10.f, true);

		// Check if in lobby
		MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (IsValid(MainGameMode))
		{
			MainGameMode->OnPlayerStart(this);
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TickKnockback(DeltaTime);
	}
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

		Input->BindAction(HitAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Hit);

		Input->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Movement
	DOREPLIFETIME(APlayerCharacter, bAllowInput);
	DOREPLIFETIME(APlayerCharacter, bCanMove);

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
	DOREPLIFETIME(APlayerCharacter, bCanSlideJump);
	DOREPLIFETIME(APlayerCharacter, bhasPlayedSlideSound);

	// Hitting
	DOREPLIFETIME(APlayerCharacter, HitDirection);
	DOREPLIFETIME(APlayerCharacter, bCanHit);

	// Knockback
	DOREPLIFETIME(APlayerCharacter, bShouldDealKB);
	DOREPLIFETIME(APlayerCharacter, CurrentKnockbackTime);
	DOREPLIFETIME(APlayerCharacter, KnockbackForce);

	// Items
	DOREPLIFETIME(APlayerCharacter, ItemEquipped);

	// Minigames
	DOREPLIFETIME(APlayerCharacter, bIsSafeFromStatue);
	DOREPLIFETIME(APlayerCharacter, PlayerScore);

	// Gamemode
	DOREPLIFETIME(APlayerCharacter, MainGameMode);
}

#pragma region Movement

void APlayerCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(GetController()) && bCanMove && bAllowInput)
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

	if (IsValid(GetController()))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void APlayerCharacter::OnJump()
{
	if (bAllowInput)
	{
		if (bCanSlideJump)
		{
			bIsAwaitingSlideJump = true;
		}
		else
		{
			ACharacter::Jump();
		}
	}
}

#pragma endregion

#pragma region Sprint

void APlayerCharacter::StartSprint()
{
	if (!HasAuthority())
	{
		Server_StartSprint();
		return;
	}

	if (GetVelocity().Size() >= 0.5 && !bIsCrouched && (GetCharacterMovement()->IsMovingOnGround() || bIsSliding))
	{
		bIsRunning = true;
	}
	else
	{
		EndSprint();
	}

	Multicast_HandleSprint();
}

void APlayerCharacter::Server_StartSprint_Implementation()
{
	StartSprint();
}

void APlayerCharacter::EndSprint()
{
	if (!HasAuthority())
	{
		Server_EndSprint();
		return;
	}

	bIsRunning = false;
	Multicast_HandleSprint();
}

void APlayerCharacter::Server_EndSprint_Implementation()
{
	EndSprint();
}

void APlayerCharacter::Multicast_HandleSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
}


#pragma endregion

#pragma region Crouch

void APlayerCharacter::StartCrouch()
{
	if (!HasAuthority())
	{
		Server_StartCrouch();
		return;
	}

	if (GetCharacterMovement()->CanCrouchInCurrentState() && bAllowInput)
	{
		// Crouch
		bIsCrouching = true;

		// Slide
		if ((bIsRunning || GetVelocity().Size() > WalkSpeed + 50.f) && CurrentSlideForce > CrouchSpeed && // Check if fast enough
			(GetCharacterMovement()->IsMovingOnGround() || bIsSliding)) // Check if grounded
		{
			// Allow Sliding
			bIsSliding = true;

			// Check if grounded
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);

			FVector StartLocation = GetMesh()->GetSocketLocation("GroundSocketTop");
			FVector EndLocation = GetMesh()->GetSocketLocation("GroundSocketBottom");

			// The line trace
			bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), EndLocation, ECC_Visibility, CollisionParams);

			// If grounded, allow slide jump
			if (bIsHit)
			{
				bCanSlideJump = true;
			}
			else
			{
				bCanSlideJump = false;
			}

			// Get Slide Direction
			SlideDirection = CurrentSlideForce * GetVelocity().GetUnsafeNormal();
			if (bIsAwaitingSlideJump)
			{
				SlideDirection.Z = JumpForceWhileSliding;

			}
			else
			{
				SlideDirection.Z = 0.f;
			}
		}
		else
		{
			// Unslide
			bIsSliding = false;
		}
	}
	else
	{
		bIsCrouching = false;
		bIsSliding = false;
		bCanSlideJump = false;
	}

	// This should be moved into its own function
	if (!bIsSliding)
	{
		bCanMove = true;
		bUseControllerRotationYaw = true;
		bIsAwaitingSlideJump = false;
		bCanSlideJump = false;
	}

	// Call Sound
	Multicast_HandleCrouch();
}

void APlayerCharacter::Server_StartCrouch_Implementation()
{
	StartCrouch();
}

void APlayerCharacter::EndCrouch()
{
	if (!HasAuthority())
	{
		Server_EndCrouch();
		return;
	}

	// Uncrouch
	bIsCrouching = false;
	Multicast_HandleCrouch();

	// Reset slide
	CurrentSlideForce = SlideForce;
	bIsSliding = false;
	bCanSlideJump = false;
	bCanMove = true;
	bUseControllerRotationYaw = true;
	bIsAwaitingSlideJump = false;
}

void APlayerCharacter::Server_EndCrouch_Implementation()
{
	EndCrouch();
}

void APlayerCharacter::Multicast_HandleCrouch_Implementation()
{
	// Crouch
	bIsCrouching ? Crouch() : UnCrouch();

	// Slide
	if (bIsSliding)
	{
		// Add Forward Force
		LaunchCharacter(SlideDirection, true, false);

		// Add Counterforce
		CurrentSlideForce -= CounterSlideForce;

		// Disable movement when turning camera
		bUseControllerRotationYaw = false;

		// Disable jump force
		bIsAwaitingSlideJump = false;


		// Play Sound
		if (!bhasPlayedSlideSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SlideSound, GetActorLocation(), GetActorRotation(), 1.5f);
			bhasPlayedSlideSound = true;
		}

		// Add Forward Force
		LaunchCharacter(SlideDirection, true, false);
	}
	else
	{
		bhasPlayedSlideSound = false;
	}
}

#pragma endregion

#pragma region Hitting

void APlayerCharacter::Hit()
{
	if (!HasAuthority())
	{
		Server_Hit();
		return;
	}

	if (bCanHit && bAllowInput)
	{
		// Hit Delay
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &APlayerCharacter::AllowHitting, HitDelay);
		bCanHit = false;

		// Line Trace
		FVector StartLocation = Camera->GetComponentLocation();
		FVector EndLocation = StartLocation + (Camera->GetComponentRotation().Vector() * HitDistance);

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		// The line trace
		bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::White, false, 1, 0, 1);

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
					HitPlayer->Knockback(NewHitDirection, HitForce * GetEquippedItem()->GetItemValue());
					HitPlayer->SetEquippedItem(StickTagItem);
					SetEquippedItem(nullptr);

					AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
					if (IsValid(GameMode))
					{
						GameMode->PlayerTagged = HitPlayer;
					}
				}
				else
				{
					HitPlayer->Knockback(NewHitDirection, HitForce);
				}
			}
		}

		Multicast_HandleHit();
	}
}

void APlayerCharacter::Server_Hit_Implementation()
{
	Hit();
}

void APlayerCharacter::Multicast_HandleHit_Implementation()
{
	// Play sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation(), GetActorRotation(), 1.5f);

	// Animation
	UAnimInstance* AnimationInstance = GetMesh()->GetAnimInstance();
	if (IsValid(HitAnimation))
	{
		AnimationInstance->Montage_Play(HitAnimation);
	}

}

void APlayerCharacter::Knockback(FVector NewHitDirection, float NewKnockbackForce)
{
	if (!HasAuthority())
	{
		Server_Knockback(NewHitDirection, NewKnockbackForce);
		return;
	}

	HitDirection = NewHitDirection;
	KnockbackForce = NewKnockbackForce;
	CurrentKnockbackTime = 0;
	bShouldDealKB = true;
}

void APlayerCharacter::Server_Knockback_Implementation(FVector NewHitDirection, float NewKnockbackForce)
{
	Knockback(NewHitDirection, NewKnockbackForce);
}

void APlayerCharacter::TickKnockback(float DeltaTime)
{
	if (bShouldDealKB == true)
	{
		LaunchCharacter(HitDirection * KnockbackForce, true, false);

		CurrentKnockbackTime += DeltaTime;
		if (CurrentKnockbackTime >= HitKnockbackTime)
		{
			bShouldDealKB = false;
		}
	}
}

#pragma endregion

#pragma region Health

void APlayerCharacter::SubtractHealth(int32 Health)
{
	if (!HasAuthority())
	{
		Server_SubtractHealth(Health);
		return;
	}

	if (bCanTakeDamage)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - Health, 0, MaxHealth);
	}

	if (CurrentHealth <= 0)
	{
		// Die
		Die();
	}
}

void APlayerCharacter::Die()
{
	if (!HasAuthority())
	{
		Server_Die();
		return;
	}

	if (!bIsDead)
	{
		bIsDead = true;
		MainGameMode->OnPlayerDeath(this);

		// Ragdoll
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);


		if (SpectatorPawn)
		{
			APawn* Spectator = GetWorld()->SpawnActor<APawn>(SpectatorPawn, GetActorTransform());
			if (IsValid(Spectator))
			{
				GetController()->Possess(Spectator);
				GetMesh()->SetSimulatePhysics(true);
			}
		}
	}
}

void APlayerCharacter::Server_Die_Implementation()
{
	Die();
}

void APlayerCharacter::Server_SubtractHealth_Implementation(int32 Health)
{
	SubtractHealth(Health);
}

#pragma endregion

#pragma region Item Equip

void APlayerCharacter::SetEquippedItem(UItemData* Item, APlayerCharacter* ReceivingPlayer)
{
	if (!HasAuthority())
	{
		Server_SetEquippedItem(Item, ReceivingPlayer);
		return;
	}
	
	if (IsValid(Item))
	{
		ItemEquipped = Item;
		ItemMesh->SetStaticMesh(ItemEquipped->GetItemMesh());
		Multicast_SetEquippedItem(Item, ReceivingPlayer);
	}
	else
	{
		ItemEquipped = nullptr;
		ItemMesh->SetStaticMesh(nullptr);
	}
}

void APlayerCharacter::Server_SetEquippedItem_Implementation(UItemData* Item, APlayerCharacter* ReceivingPlayer)
{
	SetEquippedItem(Item, ReceivingPlayer);
}

void APlayerCharacter::Multicast_SetEquippedItem_Implementation(UItemData* Item, APlayerCharacter* ReceivingPlayer)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ItemEquippedSound, GetActorLocation(), GetActorRotation(), 1.0f);
}

#pragma endregion

#pragma region Interact

void APlayerCharacter::Interact()
{
	if (!HasAuthority())
	{
		Server_Interact();
		return;
	}

	TArray<FHitResult> HitResults;

	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), GetActorLocation(),
		FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(InteractRange));

	//DrawDebugSphere(GetWorld(), GetActorLocation(), InteractRange, 20, FColor::Purple, true, 1.f);

	if (bHit)
	{
		for (FHitResult Hit : HitResults)
		{
			IInteractableInterface* Interactable = Cast<IInteractableInterface>(Hit.GetActor());
			if (Interactable)
			{
				Interactable->Interact(this);
				break;
			}
		}

	}
}

void APlayerCharacter::Server_Interact_Implementation()
{
	Interact();
}

#pragma endregion

#pragma region Minigames

void APlayerCharacter::SetPlayerScore(float NewScore)
{
	if (!HasAuthority())
	{
		Server_SetPlayerScore(NewScore);
		return;
	}

	PlayerScore = NewScore;
}

void APlayerCharacter::Server_SetPlayerScore_Implementation(float NewScore)
{
	SetPlayerScore(NewScore);
}

#pragma endregion


#pragma region Server Heartbeat

void APlayerCharacter::SendHeartbeatToServer()
{
	if (!HasAuthority())
	{
		Server_SendHeartbeatToServer();
		return;
	}

	AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(GameMode))
	{
		GameMode->ReceiveHeartbeat(this);
	}
}

void APlayerCharacter::Server_SendHeartbeatToServer_Implementation()
{
	SendHeartbeatToServer();
}

#pragma endregion
