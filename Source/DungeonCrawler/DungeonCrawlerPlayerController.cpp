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

	InputComponent->BindAction("SetDestination", IE_Released, this, &ADungeonCrawlerPlayerController::MoveToMouseCursor);
}

void ADungeonCrawlerPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	// When moving check if you reached destination
	if (BeginMoving) {
		if (!CalcDistance()) { return; }

		// If the player reached the destination end the round
		if (Distance <= 120.0f) {
			BeginMoving = false;
			FinishRound.Broadcast();
			FinishRound.Clear();
			IsYourRound = false;
		}
	}
}

void ADungeonCrawlerPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Calc max distance by speed and margin
	MaxDistance = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;

	// Bind round based system event to BeginRound
	ADungeonCrawlerGameMode* GameMode = (ADungeonCrawlerGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &ADungeonCrawlerPlayerController::BeginRound);
}


void ADungeonCrawlerPlayerController::BeginRound(FString name) {
	if (name == PlayerName) {
		IsYourRound = true;
	}
}

// Activates when mouse button press is released
void ADungeonCrawlerPlayerController::MoveToMouseCursor() {
	if (!IsYourRound) { return; }

	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		DestLocation = Hit.ImpactPoint;
		SetNewMoveDestination();
	}
}

void ADungeonCrawlerPlayerController::SetNewMoveDestination() {
	if (!CalcDistance()) { return; }
	// We need to issue move command only if far enough in order for walk animation to play correctly
	if (Distance > 120.0f && Distance <= MaxDistance)
	{
		// Begin moving so start tracking the distance the player needs yet to walk/ run
		BeginMoving = true;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
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
