// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuickProjectGameMode.h"
#include "QuickProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AQuickProjectGameMode::AQuickProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
