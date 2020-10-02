#include "EnemyAIController.h"
#include "DungeonCrawlerGameMode.h"
#include "Runtime\AIModule\Classes\Blueprint\AIBlueprintHelperLibrary.h"


AEnemyAIController::AEnemyAIController() {

}

void AEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (BeginMoving) {
		if (!CalcDistance()) { return; }

		if (Distance <= 130.0f) {
			BeginMoving = false;
			StopMovement();
			FinishRound.Broadcast();
			FinishRound.Clear();
		}
	}
}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	AActor* ActorCharacter = GetPawn();
	Name = ActorCharacter->Tags[1].ToString();

	// Bind round based system event to BeginRound
	ADungeonCrawlerGameMode* GameMode = (ADungeonCrawlerGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &AEnemyAIController::BeginRound);
}

void AEnemyAIController::BeginRound(FString name) {
	if (name == Name) {
		DestLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		BeginMoving = true;
		MoveToLocation(DestLocation);
	}
}

bool AEnemyAIController::CalcDistance() {
	APawn* const MyPawn = GetPawn();
	if (MyPawn) {
		Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
		return true;
	}

	return false;
}