// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsulePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"

void ACapsulePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	if (ChargeSliderWidgetClass) // Vérifie si la classe du widget est bien assignée
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "ChargeSliderWidgetClass");
		ChargeSliderWidget = CreateWidget<UUserWidget>(this, ChargeSliderWidgetClass);

		if (ChargeSliderWidget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "Display Widget");
			ChargeSliderWidget->AddToViewport();  // Ajoute au HUD
		}
	}
}