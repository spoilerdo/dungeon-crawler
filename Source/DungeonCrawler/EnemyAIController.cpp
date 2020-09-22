#include "EnemyAIController.h"
#include "DungeonCrawlerGameMode.h"
#include "Runtime\AIModule\Classes\Blueprint\AIBlueprintHelperLibrary.h"

AEnemyAIController::AEnemyAIController() {

}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	// Bind round based system event to BeginRound
	ADungeonCrawlerGameMode* GameMode = (ADungeonCrawlerGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &AEnemyAIController::BeginRound);
}

void AEnemyAIController::BeginRound(FString name) {
	if (name == Name) {
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		MoveToLocation(PlayerLocation);

		FinishRound.Broadcast();
	}
}