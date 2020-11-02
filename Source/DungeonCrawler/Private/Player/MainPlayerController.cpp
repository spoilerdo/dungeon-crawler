#include "Player/MainPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/DecalComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "World/RoundBasedGameMode.h"
#include "Enemy/EnemyCharacter.h"
#include "UI/UIOverlay.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Engine/World.h"

AMainPlayerController::AMainPlayerController() {
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	ConstructorHelpers::FClassFinder<UUIOverlay> UIOverlayClass(TEXT("/Game/UI/WBP_GameOverlay.WBP_GameOverlay_C"));
	if (UIOverlayClass.Succeeded()) {
		UIOverlayTClass = UIOverlayClass.Class;
	}
}

void AMainPlayerController::SetupInputComponent() {
	// Set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_DoubleClick, this, &AMainPlayerController::MoveToMouseCursor);
	InputComponent->BindAction("SetAttackGoal", IE_Released, this, &AMainPlayerController::SetAttackGoal);
}

void AMainPlayerController::DisableController(class APlayerController* PlayerController) {
	DisableInput(PlayerController);
	InputDisabled = true;
}

void AMainPlayerController::PlayerTick(float DeltaTime) {
	Super::PlayerTick(DeltaTime);

	// When moving check if you reached destination
	if (CurrentAction == 'M') {
		if (!CalcDistance(GetPawn())) { return; }

		// If the player reached the destination go to the next phase
		if (Distance <= 120.0f && SpeedLeft <= 120.0f) {
			NextPhase();
		}
	}
}

void AMainPlayerController::BeginPlay() {
	Super::BeginPlay();

	UIOverlay = CreateWidget<UUIOverlay>(this, UIOverlayTClass);
	UIOverlay->AddToViewport(9999);

	// Calc max walk distance by speed and margin
	Speed = (Speed * 100) + (Speed * 100 / 2) + SpeedToWorldMargin;
	SpeedLeft = Speed;
	// Calc max attack distance by attack range and margin
	AttackRange -= 1;
	AttackRange = (AttackRange * 100) + (AttackRange * 100 / 2) + AttackToWorldMargin;

	// Bind round based system event to BeginRound method
	ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ActivateRound.AddUObject(this, &AMainPlayerController::BeginRound);
}

void AMainPlayerController::BeginRound(const FString& name) {
	if (name == PlayerName) {
		CurrentAction = 'M';
		DisplayNextPhase("Moving phase");
	}
}

void AMainPlayerController::MoveToMouseCursor() {
	if (CurrentAction != 'M' && CurrentAction != 'A') return;

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
	if (!CalcDistance(GetPawn())) return;

	// We need to issue move command only if far enough in order for walk animation to play correctly
	if (Distance > 120.0f && Distance <= SpeedLeft) {
		SpeedLeft -= Distance;
		DisplaySpeedLeft();

		// Begin moving so start tracking the distance the player needs yet to walk/ run
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
	}
}

void AMainPlayerController::DisplaySpeedLeft() {
	UWidget* bar = UIOverlay->GetWidgetFromName("StaminaBar");
	float progress = SpeedLeft / (float)Speed;
	if (progress <= 0.150) { progress = 0; }

	if (bar != NULL) Cast<UProgressBar>(bar)->SetPercent(progress);
}

void AMainPlayerController::SetAttackGoal() {
	if (CurrentAction != 'A') return;
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	DestLocation = Hit.ImpactPoint;
	if (!CalcDistance(GetPawn())) return;

	if (Hit.bBlockingHit && Hit.GetActor()->ActorHasTag("Enemy") && Distance <= AttackRange) {
		// We hit an enemy and its in range
		UpdateRenderCustomDepth(false);

		AttackGoal = Cast<AEnemyCharacter>(Hit.GetActor());
		UpdateRenderCustomDepth(true);
	}
}

void AMainPlayerController::UpdateRenderCustomDepth(const bool& DepthValue) {
	if (!AttackGoal) return;
	USkeletalMeshComponent* Mesh = AttackGoal->GetMesh();
	if (Mesh != NULL) {
		Mesh->SetRenderCustomDepth(DepthValue);
	}
}

void AMainPlayerController::Attack() {
	if (CurrentAction != 'A' || AttackGoal == NULL) return;

	const int hit = FMath::RandRange(5, 10);
	AttackGoal->DoDamage(hit, Damage);

	NextPhase();
}

void AMainPlayerController::NextPhase() {
	if (CurrentAction == 'M') {
		// Second phase
		SpeedLeft = Speed;
		DisplaySpeedLeft();
		CurrentAction = 'A';
		DisplayNextPhase("Attack phase");
		ToggleAttackUI(true);
	}
	else if (CurrentAction == 'A') {
		// End phase
		CurrentAction = NULL;
		DisplayNextPhase("Enemy phase");
		ToggleAttackUI(false);
		UpdateRenderCustomDepth(false);
		FinishRound.Broadcast();
		FinishRound.Clear();
	}
}

void AMainPlayerController::DisplayNextPhase(const FString& Phase) {
	UWidget* cPhaseText = UIOverlay->GetWidgetFromName("CurrentPhaseText");
	if(cPhaseText != NULL) Cast<UTextBlock>(cPhaseText)->SetText(FText::FromString(Phase));

	UWidget* nPhaseText = UIOverlay->GetWidgetFromName("NextPhaseText");
	if(nPhaseText != NULL) Cast<UTextBlock>(nPhaseText)->SetText(FText::FromString(Phase));

	UIOverlay->OnChangePhase();
}

void AMainPlayerController::ToggleAttackUI(const bool& Visible) {
	// TODO: fix when attack panel is added
	UWidget* button = UIOverlay->GetWidgetFromName("AttackButton");
	if(button != NULL) Cast<UButton>(button)->SetVisibility(Visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}