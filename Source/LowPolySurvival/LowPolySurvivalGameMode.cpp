// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LowPolySurvivalGameMode.h"
#include "LowPolySurvivalHUD.h"
#include "LowPolySurvivalCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALowPolySurvivalGameMode::ALowPolySurvivalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Character/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALowPolySurvivalHUD::StaticClass();
}
