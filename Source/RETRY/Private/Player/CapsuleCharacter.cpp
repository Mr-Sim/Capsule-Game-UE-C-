// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CapsuleCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"

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
}

//////////////////////////////////////////////////////////////////////////// Input

void ACapsuleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Move);

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
