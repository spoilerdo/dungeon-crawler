// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCrawlerGameMode.h"
#include "DungeonCrawlerPlayerController.h"
#include "DungeonCrawlerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime\Engine\Classes\Kismet\GameplayStatics.h"

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

void ADungeonCrawlerGameMode::StartPlay() {
	rounds.Init("P1", 1);
	PlayRound();
}

void ADungeonCrawlerGameMode::PlayRound() {
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *rounds[0], foundActors);

	if (foundActors.Num() > 0) {
		const AActor* actor = foundActors[0];
		ADungeonCrawlerPlayerController* PC = Cast<ADungeonCrawlerPlayerController>(UGameplayStatics::GetPlayerController(actor, 0));
		if (PC != NULL) {
			PC->FinishRound.AddUObject(this, &ADungeonCrawlerGameMode::EndRound);
		}

		ActivateRound.Broadcast(*rounds[0]);
	}
}

void ADungeonCrawlerGameMode::EndRound() {
	UE_LOG(LogTemp, Warning, TEXT("END ROUND"));
}

