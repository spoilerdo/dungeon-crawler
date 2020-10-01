// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCrawlerPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "DungeonCrawlerGameMode.h"
#include "DungeonCrawlerCharacter.h"
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

		// If the player reached the destination end the round
		if (Distance <= 120.0f) {
			CurrentAction = 'A';
		}
	}
}

void ADungeonCrawlerPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Calc max walk distance by speed and margin
	Speed = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	// Calc max attack distance by speed and margin
	AttackRange = (AttackRange * 100) + (AttackRange * 100 / 2) + AttackToWorldMargin;

	// Bind round based system event to BeginRound
	ADungeonCrawlerGameMode* GameMode = (ADungeonCrawlerGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &ADungeonCrawlerPlayerController::BeginRound);
}

void ADungeonCrawlerPlayerController::BeginRound(FString name) {
	if (name == PlayerName) {
		IsYourRound = true;
		CurrentAction = 'M';
	}
}

// Activates when mouse button press is released
void ADungeonCrawlerPlayerController::MoveToMouseCursor() {
	if (!IsYourRound) { return; }

	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit) {
		// We hit something, move there
		DestLocation = Hit.ImpactPoint;
		SetNewMoveDestination();
	}
}

void ADungeonCrawlerPlayerController::SetNewMoveDestination() {
	if (!CalcDistance()) { return; }
	// We need to issue move command only if far enough in order for walk animation to play correctly
	if (Distance > 120.0f && Distance <= Speed)
	{
		// Begin moving so start tracking the distance the player needs yet to walk/ run
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		if (CurrentAction == 'A') {
			EndRound();
			return;
		}
		CurrentAction = 'M';
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

void ADungeonCrawlerPlayerController::SetAttackGoal() {
	if (!IsYourRound || CurrentAction != 'A') { return; }
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	DestLocation = Hit.ImpactPoint;
	if (!CalcDistance()) { return; }

	if (Hit.bBlockingHit && Hit.GetActor()->ActorHasTag("Enemy") && Distance <= AttackRange) {
		// We hit an enemy and its in range
		if (AttackGoal != NULL) {
			UpdateRenderCustomDepth(false);
		}
		AttackGoal = Cast<ACharacter>(Hit.GetActor());
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
}

void ADungeonCrawlerPlayerController::EndRound() {
	FinishRound.Broadcast();
	FinishRound.Clear();
	IsYourRound = false;
}