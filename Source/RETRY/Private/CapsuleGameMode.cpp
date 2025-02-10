// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsuleGameMode.h"

ACapsuleGameMode::ACapsuleGameMode() : Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/CapsuleGame/Blueprints/Player/BP_CapsulePlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}