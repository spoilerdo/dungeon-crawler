// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCrawlerGameMode.h"
#include "DungeonCrawlerPlayerController.h"
#include "DungeonCrawlerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADungeonCrawlerGameMode::ADungeonCrawlerGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADungeonCrawlerPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Player/Blueprints/PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}