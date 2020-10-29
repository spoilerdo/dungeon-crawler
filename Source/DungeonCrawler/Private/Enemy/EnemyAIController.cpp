#include "Enemy/EnemyAIController.h"
#include "World/RoundBasedGameMode.h"
#include "Player/MainPlayerCharacter.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"

AEnemyAIController::AEnemyAIController() {

}

void AEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (MyTurn) {
		if (!CalcDistance(GetPawn())) return;
		
		// If target is in range with the given Speed, walk until the attack range is met or it is adjacent.
		if (TargetInRange && (Distance <= AttackRange || Distance <= 120.0f)) {
			StopMovement();
			Attack();
		}
		// Target is not in range so walk until no Speed is left
		else if (!TargetInRange) {
			if (Distance <= Speed) {
				StopMovement();
				Attack();
			}
		}
	}
}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	// Calc max walk distance by speed and margin
	Speed = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	// Calc max attack distance by attack range and margin
	AttackRange -= 1;
	AttackRange = (AttackRange * 100) + (AttackRange * 100 / 2) + AttackToWorldMargin;

	AActor* ActorCharacter = GetPawn();
	Name = ActorCharacter->Tags[1].ToString();

	// Bind round based system event to BeginRound
	ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &AEnemyAIController::BeginRound);
}

void AEnemyAIController::BeginRound(const FString& name) {
	if (name == Name) {
		if (!CalcDistance(GetPawn())) return;
		// Check if target is in range
		TargetInRange = Distance <= Speed;

		// First phase is the movind phase
		Move();
		MyTurn = true;
	}
}

// Move to the player (target)
void AEnemyAIController::Move() {
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController()) {
		DestLocation = PC->GetPawn()->GetActorLocation();
		MoveToLocation(DestLocation);
	}
}

void AEnemyAIController::Attack() {
	if (Distance <= AttackRange) {
		AMainPlayerCharacter* AttackGoal = Cast<AMainPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (AttackGoal) {
			const int hit = FMath::RandRange(2, 10);
			AttackGoal->DoDamage(hit, Damage);
		}
	}

	EndRound();
}

void AEnemyAIController::EndRound() {
	MyTurn = false;
	FinishRound.Broadcast();
	FinishRound.Clear();
}