// Copyright Epic Games, Inc. All Rights Reserved.

#include "TankGameGameMode.h"
#include "TankGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATankGameGameMode::ATankGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
