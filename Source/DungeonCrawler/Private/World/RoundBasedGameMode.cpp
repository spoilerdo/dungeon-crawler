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
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Player/Blueprints/PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

// Start playing the game
void ARoundBasedGameMode::StartPlay() {
	Super::StartPlay();	
	
	Rounds.Init("P1", 1);
	Rounds.Push("E1");
	Rounds.Push("E2");
	// Load in first round
	PlayRound();
}

void ARoundBasedGameMode::PlayRound() {
	// Check if rounds are ended if it is, reset it
	if (Index >= Rounds.Num()) { Index = 0; }

	// Get next player
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), *Rounds[Index], foundActors);

	if (foundActors.Num() > 0) {
		AActor* actor = foundActors[0];
		
		if (Rounds[Index].Contains("P")) {
			AMainPlayerController* PC = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(actor, 0));
			if (PC != NULL) {
				// Bind the player's end round event to EndRound
				PC->FinishRound.AddUObject(this, &ARoundBasedGameMode::EndRound);
			}
		}
		else {
			AEnemyAIController* AC = Cast<AEnemyAIController>(UAIBlueprintHelperLibrary::GetAIController(actor));
			if (AC != NULL) {
				AC->FinishRound.AddUObject(this, &ARoundBasedGameMode::EndRound);
			}
		}

		// Activate the round by broadcasting event to all controllers
		ActivateRound.Broadcast(Rounds[Index]);
	}
}

void ARoundBasedGameMode::EndRound() {
	// Check if objective is met


	// Got to the next round
	Index += 1;
	PlayRound();
}

void ARoundBasedGameMode::CheckObjective() {

}

UFUNCTION(exec)
void ARoundBasedGameMode::UpdateObjective(FString& EnemyTag, int16& NewEnemyObjectives, FVector& NewDestination) {
	if (NewEnemyObjectives) {

	}
}

void ARoundBasedGameMode::DeleteRound(FString& Tag) {
	Rounds.RemoveSingle(Tag);
	FString test = Rounds.Last();
}