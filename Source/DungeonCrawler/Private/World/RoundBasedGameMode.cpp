#include "World/RoundBasedGameMode.h"
#include "Player/MainPlayerController.h"
#include "Enemy/EnemyAIController.h"
#include "Player/MainPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ARoundBasedGameMode::ARoundBasedGameMode() {
	// Use custom PlayerController class
	PlayerControllerClass = AMainPlayerController::StaticClass();

	// Set default pawn class to Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Player/Blueprints/PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

// Start playing the game
void ARoundBasedGameMode::StartPlay() {
	Super::StartPlay();	
	
	rounds.Init("P1", 1);
	rounds.Push("E1");
	rounds.Push("E2");
	// Load in first round
	PlayRound();
}

void ARoundBasedGameMode::PlayRound() {
	// Check if rounds are ended if it is, reset it
	if (index >= rounds.Num()) { index = 0; }

	// Get next player
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *rounds[index], foundActors);

	if (foundActors.Num() > 0) {
		AActor* actor = foundActors[0];
		
		if (rounds[index].Contains("P")) {
			AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(actor, 0));
			if (PC != NULL) {
				// Bind the player's end round event to EndRound
				PC->FinishRound.AddUObject(this, &ARoundBasedGameMode::EndRound);
			}
		}
		else {
			AEnemyAIController* AC = Cast<AEnemyAIController>(UAIBlueprintHelperLibrary::GetAIController(actor));
			if (AC != NULL) {
				AC->FinishRound.AddUObject(this, &ARoundBasedGameMode::EndRound);
			}
		}

		// Activate the round by broadcasting event to all controllers
		ActivateRound.Broadcast(*rounds[index]);
	}
}

void ARoundBasedGameMode::EndRound() {
	// Got to the next round
	index += 1;
	PlayRound();
}