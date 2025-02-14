// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CapsuleCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/DashComponent.h"
#include "Player/JumpComponent.h"

//////////////////////////////////////////////////////////////////////////
// ARETRYCharacter

ACapsuleCharacter::ACapsuleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	InitialFOV = 90.f;
	InitialSpringArmTarget = 0.f;
	FovTimeline = FTimeline();
	SpringArmTimeline = FTimeline();
	
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

	// Create Dash Component
	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));
	JumpComponent = CreateDefaultSubobject<UJumpComponent>(TEXT("JumpComponent"));

	// Setup Character Physics
	GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	GetCharacterMovement()->FallingLateralFriction = 0.f;
	GetCharacterMovement()->BrakingFriction = 0.f;
	

}

void ACapsuleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (CameraComponent)
	{
		InitialFOV = CameraComponent->FieldOfView;
	}

	if (!FovCurve || !SpringArmCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Courbes d'animation manquantes pour les timelines"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "AAAAAAAAAAA");

	}
	
	if (FovCurve)
	{
		FOnTimelineFloat FovTimelineCallback;
		FovTimelineCallback.BindUFunction(this, FName("UpdateFov"));
		FovTimeline.AddInterpFloat(FovCurve, FovTimelineCallback);
		FovTimeline.SetLooping(false);
	}

	if (SpringArmCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpringArmCurve keys count: %d"), SpringArmCurve->FloatCurve.GetNumKeys());

		GEngine->AddOnScreenDebugMessage(8, 5.f, FColor::Green, "SpringArmOk");
		FOnTimelineFloat SpringArmTimelineCallback;
		SpringArmTimelineCallback.BindUFunction(this, FName("UpdateSpringArm"));
		SpringArmTimeline.AddInterpFloat(SpringArmCurve, SpringArmTimelineCallback);
		SpringArmTimeline.SetLooping(false);
	}
}

void ACapsuleCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Green, "Endplay");

	Super::EndPlay(EndPlayReason);
	if (FovTimeline.IsPlaying())
		FovTimeline.Stop();
	if (SpringArmTimeline.IsPlaying())
		SpringArmTimeline.Stop();
}

void ACapsuleCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Mise à jour des timelines
	FovTimeline.TickTimeline(DeltaSeconds);
	SpringArmTimeline.TickTimeline(DeltaSeconds);
	
	GEngine->AddOnScreenDebugMessage(10, 0.0f, FColor::Blue, FString::Printf(TEXT("SpringArm Length: %f"), SpringArm->TargetArmLength));
}

//////////////////////////////////////////////////////////////////////////// Input

void ACapsuleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{


		// Stomp
		EnhancedInputComponent->BindAction(StompAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Stomp);
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Move);
		MoveActionBinding = &EnhancedInputComponent->BindActionValue(MoveAction);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACapsuleCharacter::Look);

		if (DashComponent)
		{
			DashComponent->SetupDashInput(EnhancedInputComponent);
		}

		if (JumpComponent)
		{
			JumpComponent->SetupJumpInput(EnhancedInputComponent);
		}
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

	if (DashComponent)
	{
		DashComponent->SetInputDirection(Value.Get<FVector2D>());
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

void ACapsuleCharacter::Stomp()
{
}


void ACapsuleCharacter::SetFirstPerson()
{
	bIsFirstPerson = true;
	SpringArm->TargetArmLength = 500.f;
	Mesh3P->SetOwnerNoSee(true);
}

void ACapsuleCharacter::SetThirdPerson()
{
	bIsFirstPerson = false;
	SpringArm->TargetArmLength = 500.f;
	Mesh3P->SetOwnerNoSee(false);
}

void ACapsuleCharacter::VelocityAnimation(float Force)
{
	if (!CameraComponent || !SpringArm) 
	{
		return GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Camera component or springarm absent for VelocityAnimation");
	}

	if (FovTimeline.IsPlaying() || SpringArmTimeline.IsPlaying())
	{
		FovTimeline.Stop();
		SpringArmTimeline.Stop();
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);

	TargetSpringArmTarget = Force;

	// Config Timeline 
	FovTimeline.SetPlayRate(1.f / 1.f);
	SpringArmTimeline.SetPlayRate(1.f / 1.f);

	// Begin anim
	FovTimeline.PlayFromStart();
	SpringArmTimeline.PlayFromStart();

	// Inverse
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&]()
	{
		UE_LOG(LogTemp, Warning, TEXT("Retour à la normale"));
		//FovTimeline.Reverse();
		//SpringArmTimeline.Reverse();
	}, 0.8f, false);
}

void ACapsuleCharacter::UpdateFov(float Value)
{
	GEngine->AddOnScreenDebugMessage(6, 5.f, FColor::Green, "Update FOV");

	if (CameraComponent)
	{
		CameraComponent->SetFieldOfView(FMath::Lerp(InitialFOV, TargetFov, Value));
	}
}

void ACapsuleCharacter::UpdateSpringArm(float Value)
{
	GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Green, "Update Spring Arm");
	if (SpringArm)
	{
		SpringArm->TargetArmLength = FMath::Lerp(InitialSpringArmTarget, TargetSpringArmTarget, Value);
	}
}

void ACapsuleCharacter::ResetCamera()
{
}

void ACapsuleCharacter::CancelAllTimers()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
}
