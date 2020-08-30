// Copyright Epic Games, Inc. All Rights Reserved.

#include "UEPrototypeGameMode.h"
#include "UEPrototypeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUEPrototypeGameMode::AUEPrototypeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
