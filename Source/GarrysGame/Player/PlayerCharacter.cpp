#include "../Player/PlayerCharacter.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	bUseControllerRotationYaw = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Speeds
	CrouchSpeed = 400.f;
	WalkSpeed = 400.f;
	RunSpeed = 800.f;
	SlideForce = 1000.f;
	CounterSlideForce = 1.f;

	// Movement
	bCanMove = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Speeds
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	CurrentSlideForce = SlideForce;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);

		Input->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartSprint);
		Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndSprint);

		Input->BindAction(SlideAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StartCrouch);
		Input->BindAction(SlideAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndCrouch);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate Current Health
	DOREPLIFETIME(APlayerCharacter, CurrentHealth);
	DOREPLIFETIME(APlayerCharacter, CurrentSlideForce);
	//DOREPLIFETIME(APlayerCharacter, bIsRunning);
}

void APlayerCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller) && bCanMove)
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

void APlayerCharacter::Jump()
{
	ACharacter::Jump();
}

#pragma region Sprint
void APlayerCharacter::StartSprint_Implementation()
{
	if (GetVelocity().Size() >= 0.5 && !GetCharacterMovement()->IsCrouching())
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
	if (bIsRunning == true)
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

void APlayerCharacter::StartCrouch()
{
	if (GetCharacterMovement()->CanCrouchInCurrentState())
	{
		// Crouch
		Crouch();

		// Slide
		if (GetVelocity().Size() >= WalkSpeed + 20.f)
		{
			// Add Forward Force
			GetCharacterMovement()->Launch(CurrentSlideForce * GetActorForwardVector());

			// Add Counterforce
			CurrentSlideForce -= CounterSlideForce;

			// Disable Movement
			bCanMove = false;
			bUseControllerRotationYaw = false;
		}
	}
}

void APlayerCharacter::EndCrouch()
{
	// Uncrouch
	UnCrouch();

	// Unslide
	bCanMove = true;
	bUseControllerRotationYaw = true;
	CurrentSlideForce = SlideForce;
}
#pragma endregion
