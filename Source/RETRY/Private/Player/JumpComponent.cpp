#include "Player/JumpComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CapsuleCharacter.h"

UJumpComponent::UJumpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentJumpForce = BaseJumpForce;
	bIsJumping = false;
}

void UJumpComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACapsuleCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("JumpComponent: Owner is not a CapsuleCharacter!"));
	}
}

void UJumpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Blue, FString::Printf(TEXT("JumpForce: %f"), CurrentJumpForce));
}

void UJumpComponent::SetupJumpInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACapsuleCharacter>(GetOwner());
	}
	
	if (!OwnerCharacter || !EnhancedInputComponent) return;

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UJumpComponent::SimpleJump);
	EnhancedInputComponent->BindAction(ChargedJumpAction, ETriggerEvent::Ongoing, this, &UJumpComponent::ChargeJump);
	EnhancedInputComponent->BindAction(ChargedJumpAction, ETriggerEvent::Completed, this, &UJumpComponent::ReleaseJump);
}

void UJumpComponent::ChargeJump()
{
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()->IsMovingOnGround()) return;

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	CurrentJumpForce = FMath::Clamp(CurrentJumpForce + JumpChargeRate * DeltaTime, BaseJumpForce, MaxJumpForce);
}

void UJumpComponent::ReleaseJump()
{
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()->IsMovingOnGround()) return;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "ChargedJump");
	ExecuteJump();
	PlayJumpAnimation(CurrentJumpForce);
	OwnerCharacter->VelocityAnimation(BaseJumpForce);
	
}

void UJumpComponent::SimpleJump()
{
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement()->IsMovingOnGround()) return;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "SimpleJump");

	CurrentJumpForce = BaseJumpForce;
	ExecuteJump();
}

void UJumpComponent::ExecuteJump()
{
	if (!OwnerCharacter) return;

	// Appliquer la force de saut
	OwnerCharacter->LaunchCharacter(FVector(0, 0, CurrentJumpForce), true, true);

	PlayJumpAnimation(CurrentJumpForce);

	CurrentJumpForce = BaseJumpForce;
	bIsJumping = true;
}

void UJumpComponent::PlayJumpAnimation(float Force)
{
	if (OwnerCharacter->GetMesh3P())
	{
		// Jouer une animation de saut si disponible
		OwnerCharacter->GetMesh3P()->PlayAnimation(JumpAnim, false);
		OwnerCharacter->VelocityAnimation(Force * 0.4f);
	}
}
