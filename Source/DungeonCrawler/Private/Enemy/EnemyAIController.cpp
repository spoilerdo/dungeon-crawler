#include "Enemy/EnemyAIController.h"
#include "World/RoundBasedGameMode.h"
#include "Player/MainPlayerCharacter.h"
#include "Runtime/AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"


AEnemyAIController::AEnemyAIController() {

}

void AEnemyAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (MyTurn) {
		if (!CalcDistance()) return;
		
		// TODO fix this if statement
		if (Distance <= Speed || Distance <= 130.0f) {
			UE_LOG(LogTemp, Warning, TEXT("ATTACK"));
			StopMovement();
			Attack();
		}
	}
}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();

	// Calc max walk distance by speed and margin
	Speed = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	// Calc max attack distance by attack range and margin
	AttackRange = (AttackRange * 100) + (AttackRange * 100 / 2) + AttackToWorldMargin;

	AActor* ActorCharacter = GetPawn();
	Name = ActorCharacter->Tags[1].ToString();

	// Bind round based system event to BeginRound
	ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &AEnemyAIController::BeginRound);
}

void AEnemyAIController::BeginRound(FString name) {
	if (name == Name) {
		Move();
		MyTurn = true;
	}
}

void AEnemyAIController::Move() {
	DestLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	MoveToLocation(DestLocation);
}

bool AEnemyAIController::CalcDistance() {
	APawn* const MyPawn = GetPawn();
	if (MyPawn) {
		Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
		return true;
	}

	return false;
}

void AEnemyAIController::Attack() {
	if (Distance <= AttackRange) {
		AMainPlayerCharacter* AttackGoal = Cast<AMainPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (AttackGoal) {
			const int hit = FMath::RandRange(2, 10);
			AttackGoal->DoDamage(hit, 1);
		}
	}

	EndRound();
}

void AEnemyAIController::EndRound() {
	MyTurn = false;
	FinishRound.Broadcast();
	FinishRound.Clear();
}