// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CapsuleCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MaterialHLSLTree.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsAsset.h"

//////////////////////////////////////////////////////////////////////////
// ARETRYCharacter

ACapsuleCharacter::ACapsuleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 0.f;
	SpringArm->bAutoActivate = true;
	CameraComponent->SetupAttachment(SpringArm);
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(RootComponent);
	Mesh3P->bCastDynamicShadow = true;
	Mesh3P->CastShadow = true;


}

void ACapsuleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	SetThirdPerson();

	JumpForce = BaseJumpForce;
	DashForce = BaseDashForce;
}

void ACapsuleCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Blue, FString::Printf(TEXT("JumpForce: %f"), JumpForce));
	GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Blue, FString::Printf(TEXT("DashForce: %f"), DashForce));
}

//////////////////////////////////////////////////////////////////////////// Input

void ACapsuleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::ReleaseJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(ChargedJumpAction, ETriggerEvent::Ongoing, this, &ACapsuleCharacter::ChargeJump);
		EnhancedInputComponent->BindAction(ChargedJumpAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::ReleaseJump);
		EnhancedInputComponent->BindAction(ChargedJumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::SimpleDash);
		EnhancedInputComponent->BindAction(ChargedDashAction, ETriggerEvent::Ongoing, this, &ACapsuleCharacter::ChargeDash);
		EnhancedInputComponent->BindAction(ChargedDashAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::ReleaseDash);

		// Stomp
		EnhancedInputComponent->BindAction(StompAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Stomp);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Move);
		MoveActionBinding = &EnhancedInputComponent->BindActionValue(MoveAction);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Look);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "EnhancedInputSystemInitialization failed");
	}
}


void ACapsuleCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACapsuleCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACapsuleCharacter::ChargeJump()
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	JumpForce = FMath::Clamp(JumpForce+JumpChargePower*DeltaTime, BaseJumpForce, MaxJumpForce);
}

void ACapsuleCharacter::Stomp()
{
}

void ACapsuleCharacter::SimpleDash()
{
	FVector2D InputDirection = MoveActionBinding->GetValue().Get<FVector2D>();
	InputDirection.Normalize();

	//InputDirection = (InputDirection==FVector2D::ZeroVector) ? FVector2D(GetActorForwardVector().BackwardVector.X,GetActorForwardVector().BackwardVector.Y)  : InputDirection;
	if (InputDirection == FVector2D::ZeroVector)
	{
		InputDirection = FVector2D(GetActorForwardVector().BackwardVector.X,GetActorForwardVector().BackwardVector.Y);
	}
	FVector Direction = FVector(InputDirection.X, InputDirection.Y, 0);
	ExecuteDash(Direction);
}
void ACapsuleCharacter::ChargeDash()
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	DashForce = FMath::Clamp(DashForce+DashChargePower*DeltaTime, BaseDashForce, MaxDashForce);
}

void ACapsuleCharacter::ReleaseDash()
{
	FVector2D InputDirection = MoveActionBinding->GetValue().Get<FVector2D>();
	InputDirection.Normalize();

	//InputDirection = (InputDirection==FVector2D::ZeroVector) ? FVector2D(GetActorForwardVector().BackwardVector.X,GetActorForwardVector().BackwardVector.Y)  : InputDirection;
	if (InputDirection == FVector2D::ZeroVector)
	{
		InputDirection = FVector2D(GetActorForwardVector().BackwardVector.X,GetActorForwardVector().BackwardVector.Y);
	}

	FVector Direction = FVector(InputDirection.X, InputDirection.Y, 0);
	ExecuteDash(Direction);
}

void ACapsuleCharacter::ExecuteDash(FVector Direction)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Dash");

	FVector DashVelocity = Direction * DashForce;

	GetCharacterMovement()->Velocity += DashVelocity;
	
	DashForce = BaseDashForce;
}


void ACapsuleCharacter::ReleaseJump()
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "ReleaseJump");
	ACapsuleCharacter::Jump();
	JumpForce = BaseJumpForce;
}

void ACapsuleCharacter::SetFirstPerson()
{
	bIsFirstPerson = true;
	SpringArm->TargetArmLength = 0.f;
	Mesh3P->SetOwnerNoSee(true);
}

void ACapsuleCharacter::SetThirdPerson()
{
	bIsFirstPerson = false;
	SpringArm->TargetArmLength = 500.f;
	Mesh3P->SetOwnerNoSee(false);
}
