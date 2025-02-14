// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CapsulePlayerController.generated.h"


class UInputMappingContext;
class UUserWidget;

/**
 * 
 */
UCLASS()
class RETRY_API ACapsulePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	// Begin Actor interface
protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> ChargeSliderWidgetClass;

	UPROPERTY()
	UUserWidget* ChargeSliderWidget;

	UPROPERTY()
	float GaugeCharge;
	

	// End Actor interface

public:
	UFUNCTION(BlueprintCallable)
	inline float GetGauge() { return GaugeCharge; }
	UFUNCTION(BlueprintCallable)
	inline void SetGaugeCharge(float value) { GaugeCharge = value; }
};
