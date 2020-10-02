#include "DungeonCrawlerGameMode.h"
#include "DungeonCrawlerPlayerController.h"
#include "EnemyAIController.h"
#include "DungeonCrawlerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime\Engine\Classes\Kismet\GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ADungeonCrawlerGameMode::ADungeonCrawlerGameMode() {
	// Use custom PlayerController class
	PlayerControllerClass = ADungeonCrawlerPlayerController::StaticClass();

	// Set default pawn class to Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Player/Blueprints/PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

// Start playing the game
void ADungeonCrawlerGameMode::StartPlay() {
	Super::StartPlay();	
	
	rounds.Init("P1", 1);
	rounds.Push("E1");
	rounds.Push("E2");
	// Load in first round
	PlayRound();
}

void ADungeonCrawlerGameMode::PlayRound() {
	// Check if rounds are ended if it is, reset it
	if (index >= rounds.Num()) { index = 0; }

	// Get next player
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *rounds[index], foundActors);

	if (foundActors.Num() > 0) {
		AActor* actor = foundActors[0];
		
		if (rounds[index].Contains("P")) {
			ADungeonCrawlerPlayerController* PC = Cast<ADungeonCrawlerPlayerController>(UGameplayStatics::GetPlayerController(actor, 0));
			if (PC != NULL) {
				// Bind the player's end round event to EndRound
				PC->FinishRound.AddUObject(this, &ADungeonCrawlerGameMode::EndRound);
			}
		}
		else {
			AEnemyAIController* AC = Cast<AEnemyAIController>(UAIBlueprintHelperLibrary::GetAIController(actor));
			if (AC != NULL) {
				AC->FinishRound.AddUObject(this, &ADungeonCrawlerGameMode::EndRound);
			}
		}

		// Activate the round by broadcasting event to all controllers
		ActivateRound.Broadcast(*rounds[index]);
	}
}

void ADungeonCrawlerGameMode::EndRound() {
	// Got to the next round
	index += 1;
	PlayRound();
}

