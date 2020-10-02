#include "DungeonCrawlerPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DungeonCrawlerGameMode.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"

ADungeonCrawlerPlayerController::ADungeonCrawlerPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ADungeonCrawlerPlayerController::SetupInputComponent() {
	// Set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_DoubleClick, this, &ADungeonCrawlerPlayerController::MoveToMouseCursor);
	InputComponent->BindAction("SetAttackGoal", IE_Released, this, &ADungeonCrawlerPlayerController::SetAttackGoal);
}

void ADungeonCrawlerPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	// When moving check if you reached destination
	if (CurrentAction == 'M') {
		if (!CalcDistance()) { return; }

		// If the player reached the destination go to the next phase
		if (Distance <= 120.0f) {
			CurrentAction = 'A';
		}
	}
}

void ADungeonCrawlerPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Calc max walk distance by speed and margin
	Speed = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	// Calc max attack distance by attack range and margin
	AttackRange = (AttackRange * 100) + (AttackRange * 100 / 2) + AttackToWorldMargin;

	// Bind round based system event to BeginRound method
	ADungeonCrawlerGameMode* GameMode = (ADungeonCrawlerGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &ADungeonCrawlerPlayerController::BeginRound);
}

void ADungeonCrawlerPlayerController::BeginRound(FString name) {
	if (name == PlayerName) {
		CurrentAction = 'M';
	}
}

// Activates when mouse button is double pressed
void ADungeonCrawlerPlayerController::MoveToMouseCursor() {
	if (CurrentAction != 'M' && CurrentAction != 'A') { return; }

	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit) {
		// We hit something, move there
		DestLocation = Hit.ImpactPoint;
		Move();
	}
}

void ADungeonCrawlerPlayerController::Move() {
	if (!CalcDistance()) { return; }
	// We need to issue move command only if far enough in order for walk animation to play correctly
	if (Distance > 120.0f && Distance <= Speed) {
		// Begin moving so start tracking the distance the player needs yet to walk/ run
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		NextPhase();
	}
}

bool ADungeonCrawlerPlayerController::CalcDistance() {
	APawn* const MyPawn = GetPawn();
	if (MyPawn) {
		Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
		return true;
	}

	return false;
}

// Set attack goal (Enemy object only), is needed before pressing the attack button
void ADungeonCrawlerPlayerController::SetAttackGoal() {
	if (CurrentAction != 'A') { return; }
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	DestLocation = Hit.ImpactPoint;
	if (!CalcDistance()) { return; }

	if (Hit.bBlockingHit && Hit.GetActor()->ActorHasTag("Enemy") && Distance <= AttackRange) {
		// We hit an enemy and its in range
		if (AttackGoal != NULL) {
			UpdateRenderCustomDepth(false);
		}
		AttackGoal = Cast<AEnemyCharacter>(Hit.GetActor());
		UpdateRenderCustomDepth(true);
	}
}

void ADungeonCrawlerPlayerController::UpdateRenderCustomDepth(bool DepthValue) {
	USkeletalMeshComponent* Mesh = AttackGoal->GetMesh();
	if (Mesh != NULL) {
		Mesh->SetRenderCustomDepth(DepthValue);
	}
}

void ADungeonCrawlerPlayerController::Attack() {
	if (CurrentAction != 'A' || AttackGoal == NULL) { return; }

	const int hit = FMath::RandRange(2, 10);
	AttackGoal->DoDamage(hit, 1);

	NextPhase();
}

void ADungeonCrawlerPlayerController::NextPhase() {	
	if (CurrentAction == 'M') {
		// Second phase
		CurrentAction = 'A';
	} else if (CurrentAction == 'A') {
		// End phase
		CurrentAction = NULL;
		FinishRound.Broadcast();
		FinishRound.Clear();
	}
}