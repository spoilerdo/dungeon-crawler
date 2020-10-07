#include "Player/MainPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/TextBlock.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "World/RoundBasedGameMode.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine/World.h"

AMainPlayerController::AMainPlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMainPlayerController::SetupInputComponent() {
	// Set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_DoubleClick, this, &AMainPlayerController::MoveToMouseCursor);
	InputComponent->BindAction("SetAttackGoal", IE_Released, this, &AMainPlayerController::SetAttackGoal);
}

void AMainPlayerController::PlayerTick(float DeltaTime) {
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

void AMainPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Calc max walk distance by speed and margin
	Speed = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	// Calc max attack distance by attack range and margin
	AttackRange = (AttackRange * 100) + (AttackRange * 100 / 2) + AttackToWorldMargin;

	// Bind round based system event to BeginRound method
	ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &AMainPlayerController::BeginRound);
}

void AMainPlayerController::BeginRound(FString name) {
	if (name == PlayerName) {
		CurrentAction = 'M';
	}
}

//Activates when mouse button is double pressed
void AMainPlayerController::MoveToMouseCursor() {
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

void AMainPlayerController::Move() {
	if (!CalcDistance()) { return; }
	UTextBlock* text = Cast<UTextBlock>(UIOverlay->GetWidgetFromName("StepsText"));
	text->Text = FText().FromString("Test");
	// We need to issue move command only if far enough in order for walk animation to play correctly
	if (Distance > 120.0f && Distance <= Speed) {
		// Begin moving so start tracking the distance the player needs yet to walk/ run
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		NextPhase();
	}
}

bool AMainPlayerController::CalcDistance() {
	APawn* const MyPawn = GetPawn();
	if (MyPawn) {
		Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
		return true;
	}

	return false;
}

// Set attack goal (Enemy object only), is needed before pressing the attack button
void AMainPlayerController::SetAttackGoal() {
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

void AMainPlayerController::UpdateRenderCustomDepth(bool DepthValue) {
	USkeletalMeshComponent* Mesh = AttackGoal->GetMesh();
	if (Mesh != NULL) {
		Mesh->SetRenderCustomDepth(DepthValue);
	}
}

void AMainPlayerController::Attack() {
	if (CurrentAction != 'A' || AttackGoal == NULL) { return; }

	const int hit = FMath::RandRange(2, 10);
	AttackGoal->DoDamage(hit, 1);

	NextPhase();
}

void AMainPlayerController::NextPhase() {
	if (CurrentAction == 'M') {
		// Second phase
		CurrentAction = 'A';
	}
	else if (CurrentAction == 'A') {
		// End phase
		CurrentAction = NULL;
		FinishRound.Broadcast();
		FinishRound.Clear();
	}
}