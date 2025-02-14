#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "JumpComponent.generated.h"

class ACapsuleCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RETRY_API UJumpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJumpComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void SetupJumpInput(UEnhancedInputComponent* EnhancedInputComponent);

	void ChargeJump();
	void ReleaseJump();
	void SimpleJump();

private:
	void ExecuteJump();
	void PlayJumpAnimation(float Force);
	void SetJumpState();
	void SetNormalState();
	void ResetJumpCooldown();

	// INPUTS
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ChargedJumpAction;

	// SETTINGS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	float JumpChargeRate = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	float MaxJumpForce = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	float BaseJumpForce = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	float JumpCooldown = 2.8f;

	// VARIABLES
	float CurrentJumpForce;
	float JumpCooldownTimer = 0.0f;
	bool bIsJumping;
	bool bCanJump;

	// RÉFÉRENCES
	UPROPERTY()
	ACapsuleCharacter* OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimSequence* JumpAnim;

	FTimerHandle JumpCooldownTimerHandle;
};
