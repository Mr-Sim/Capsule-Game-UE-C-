// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


class UCameraComponent;
class UInputAction;
class ACapsuleCharacter;
class USoundCue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RETRY_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDashComponent();

protected:
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupDashInput(UEnhancedInputComponent* EnhancedInputComponent);

	void SetInputDirection(FVector2D Direction) { InputDirection = Direction; }

	void ChargeDash();
	void ReleaseDash();
	void SimpleDash();
	void ExecuteDash(FVector Direction);



private:
	// FUNCTIONS
	void SetDashState();
	void SetNormalState();
	void PlayDashAnimation(float Force);

	
	// INPUTS
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DashAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ChargedDashAction;

	UPROPERTY(EditAnywhere, Category = "Dash", meta = (EditCondition = "DashState == true"))
	UAnimationAsset* MM_DashAnim;
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (EditCondition = "DashState == true"))
	USoundCue* DashSFX;

	// SETTINGS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashChargePower = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float MaxDashForce = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float BaseDashForce = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashCooldown = 2.8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	float DashDuration = 1.0;

	// VARIABLES	
	float fDashForce = BaseDashForce;
	FVector2D InputDirection;

	float fBaseBrakeFrictionCache;
	float fBaseGroundFrictionCache;
	
	float fDashCooldownTimer = 0.f;
	bool bIsDashing = false;
	bool bCanDash = true;
	

	// REFERENCES
	UPROPERTY()
	ACapsuleCharacter* OwnerCharacter;
	UPROPERTY()
	UCameraComponent* OwnerCamera;
};
