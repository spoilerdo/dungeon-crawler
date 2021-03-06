#include "World/RoundBasedGameMode.h"
#include "Player/MainPlayerController.h"
#include "Enemy/EnemyAIController.h"
#include "Player/MainPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ARoundBasedGameMode::ARoundBasedGameMode() {
	// Use custom PlayerController class
	PlayerControllerClass = AMainPlayerController::StaticClass();

	// Set default pawn class to Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Player/Blueprints/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ARoundBasedGameMode::StartPlay() {
	Super::StartPlay();	
	
	if(Rounds.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("No rounds specified!! Cannot play a game with no rounds"));
		return;
	}

	// Load in first round
	PlayRound();
}

void ARoundBasedGameMode::PlayRound() {
	// Check if rounds are ended if it is, reset it
	if (Index >= Rounds.Num()) {
		Index = 0;
	}

	// Get next player/ enemy
	if(AActor* actor = GetActorByTag(Rounds[Index])) {
		if (Rounds[Index].Contains("P")) {
			if (AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(actor, 0))) {
				// Bind the player's end round event to EndRound
				PC->FinishRound.AddUObject(this, &ARoundBasedGameMode::EndRound);
			}
		}
		else {
			if (AEnemyAIController* AC = Cast<AEnemyAIController>(UAIBlueprintHelperLibrary::GetAIController(actor))) {
				AC->FinishRound.AddUObject(this, &ARoundBasedGameMode::EndRound);
			}
		}

		// Activate the round by broadcasting event to all controllers
		ActivateRound.Broadcast(Rounds[Index]);
	}
}

AActor* ARoundBasedGameMode::GetActorByTag(FString& Tag) {
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *Tag, foundActors);
	if (foundActors.Num() > 0) {
		return foundActors[0];
	} else {
		return NULL;
	}
}

void ARoundBasedGameMode::EndRound() {
	// Got to the next round
	Index += 1;
	PlayRound();
}

void ARoundBasedGameMode::UpdateEnemyObjective(FString& EnemyTag) {
	// Check if the enemy is part of the objective
	if (EnemyTag == EnemyObjectiveTag) {
		// Killed a new enemy and it is an objective enemy so up the counter
		CurrentAmountOfEnemyKilled++;
		if (CurrentAmountOfEnemyKilled >= AmountOfEnemyObjectives) {
			EndGame("Victory!!");
		}
	}
}

void ARoundBasedGameMode::UpdateDestinationObjective(FVector& NewDestination) {
	// Check if the player reached the destination
	if(NewDestination.Equals(Destination, 1.0f)) {
		EndGame("Victory!!");
	}
}

void ARoundBasedGameMode::EndGame(const FString& Text) {
	// First round is always the player
	if(AActor* actor = GetActorByTag(Rounds[0])) {
		if(AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(actor, 0))) {
			// Call endgame animation on UIOverlay and disable controls for player
			PC->UIOverlay->OnShowEndGamePanel(Text);
			PC->DisableController(PC);
		}
	}
}

void ARoundBasedGameMode::DeleteRound(FString& Tag) {
	if(Tag.Contains("P")) {
		EndGame("Fail!!");
		return;
	}

	Rounds.RemoveSingle(Tag);
	FString test = Rounds.Last();
}