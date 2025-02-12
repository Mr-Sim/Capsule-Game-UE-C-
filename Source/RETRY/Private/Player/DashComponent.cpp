// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DashComponent.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CapsuleCharacter.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	fDashForce = BaseDashForce;
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACapsuleCharacter>(GetOwner());
	}
	if (OwnerCharacter)
	{
		OwnerCamera = OwnerCharacter->GetCameraComponent();
	}
	
}

void UDashComponent::SetupDashInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACapsuleCharacter>(GetOwner());
	}
	
	if (!OwnerCharacter || !EnhancedInputComponent) return GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "No Owner Character or Enhanced Input Component for input setup");

	EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &UDashComponent::SimpleDash);
	EnhancedInputComponent->BindAction(ChargedDashAction, ETriggerEvent::Ongoing, this, &UDashComponent::ChargeDash);
	EnhancedInputComponent->BindAction(ChargedDashAction, ETriggerEvent::Triggered, this, &UDashComponent::ReleaseDash);
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Blue, FString::Printf(TEXT("DashForce: %f"), fDashForce));


	// DASH COOLDOWN
	// lower cooldown & enable dash again;
	fDashCooldownTimer = fDashCooldownTimer > 0.f ? fDashCooldownTimer-DeltaTime : 0.f;
	bCanDash = fDashCooldownTimer == 0.f;

	if (bIsDashing && fDashCooldownTimer < DashCooldown-DashDuration)
	{
		bIsDashing = false;
		SetNormalState();
	} 
	
}

void UDashComponent::ChargeDash()
{
	if (!bCanDash) return;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	fDashForce = FMath::Clamp(fDashForce+DashChargePower*DeltaTime, BaseDashForce, MaxDashForce);
}

void UDashComponent::ReleaseDash()
{
	if (!OwnerCharacter || !OwnerCamera) return GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "No character or camera for Dash Component");

	GEngine->AddOnScreenDebugMessage(1, 5, FColor::Blue, "ChargedDash");

	FVector Direction = OwnerCamera->GetForwardVector();
	ExecuteDash(Direction);
}

void UDashComponent::SimpleDash()
{
	if (!bCanDash) return;
	if (!OwnerCharacter) return GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "No Character for Dash Component");

	GEngine->AddOnScreenDebugMessage(2, 5, FColor::Blue, "ChargedDash");

	FVector Forward = OwnerCharacter->GetActorForwardVector();
	FVector Right = OwnerCharacter->GetActorRightVector();
	
	FVector Direction = (Forward * InputDirection.Y) + (Right * InputDirection.X);
	Direction.Normalize();
	if (Direction.IsZero()) Direction = -Forward;

	ExecuteDash(Direction);
}

void UDashComponent::ExecuteDash(FVector Direction)
{
	if (!OwnerCharacter) return GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "No Character for Dash Component");

	FVector DashVelocity = Direction * fDashForce;
	
	OwnerCharacter->LaunchCharacter(DashVelocity, true, false);
	SetDashState();
	PlayDashAnimation(fDashForce);
	fDashForce = BaseDashForce;
	bIsDashing = true;
	fDashCooldownTimer = DashCooldown;
}

void UDashComponent::PlayDashAnimation(float Force)
{
	if (MM_DashAnim)
	{
		OwnerCharacter->GetMesh3P()->PlayAnimation(MM_DashAnim, false);
		OwnerCharacter->VelocityAnimation(Force*0.5);
	}
}

void UDashComponent::SetDashState()
{
	fBaseBrakeFrictionCache = OwnerCharacter->GetCharacterMovement()->BrakingDecelerationWalking;
	OwnerCharacter->GetCharacterMovement()->BrakingDecelerationWalking = 0.f;

	fBaseGroundFrictionCache = OwnerCharacter->GetCharacterMovement()->GroundFriction;
	OwnerCharacter->GetCharacterMovement()->GroundFriction = 0.f;
}

void UDashComponent::SetNormalState()
{
	OwnerCharacter->GetCharacterMovement()->BrakingDecelerationWalking = fBaseBrakeFrictionCache;
	OwnerCharacter->GetCharacterMovement()->GroundFriction = fBaseGroundFrictionCache;
}
