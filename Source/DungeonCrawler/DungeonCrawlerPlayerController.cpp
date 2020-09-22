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

void ADungeonCrawlerPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ADungeonCrawlerPlayerController::BeginPlay()
{
	MaxDistance = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	/*ADungeonCrawlerGameMode* GameMode = (ADungeonCrawlerGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &ADungeonCrawlerPlayerController::BeginRound);*/
}
 
void ADungeonCrawlerPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ADungeonCrawlerPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ADungeonCrawlerPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADungeonCrawlerPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADungeonCrawlerPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ADungeonCrawlerPlayerController::OnResetVR);
}

void ADungeonCrawlerPlayerController::BeginRound(FString name) {
	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAYING"));
	if (name == PlayerName) {
		IsYourRound = true;
	}
}

void ADungeonCrawlerPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADungeonCrawlerPlayerController::MoveToMouseCursor()
{
	//if (!IsYourRound) { return; }

	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ADungeonCrawlerCharacter* MyPawn = Cast<ADungeonCrawlerCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}

	FinishRound.Broadcast();
}

void ADungeonCrawlerPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	//if (!IsYourRound) { return; }

	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}

	FinishRound.Broadcast();
}

void ADungeonCrawlerPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (Distance > 120.0f && Distance <= MaxDistance)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ADungeonCrawlerPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ADungeonCrawlerPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
