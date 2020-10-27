#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RoundBasedGameMode.generated.h"

DECLARE_EVENT_OneParam(ARoundBasedGameMode, FOnActivateRound, const FString&);

UCLASS()
class DUNGEONCRAWLER_API ARoundBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARoundBasedGameMode();

	// Round based system variables
	FOnActivateRound ActivateRound;
	UPROPERTY(EditAnywhere, Category = "Round base stats")
	TArray<FString> Rounds;

	// Objective variables
	UPROPERTY(EditAnywhere, Category = "Objective")
	FString EnemyObjectiveTag;
	UPROPERTY(EditAnywhere, Category = "Objective")
	int16 AmountOfEnemyObjectives;
	UPROPERTY(EditAnywhere, Category = "Objective")
	FVector Destination;

	// Delete a player or enemy from the round
	void DeleteRound(FString& Tag);
	// Update the objectives when the player kills an enemy
	void UpdateEnemyObjective(FString& EnemyTag);
	// Update the objectives when the player reaches a new destination
	void UpdateDestinationObjective(FVector& NewDestination);
private:
	int16 Index;

	int16 CurrentAmountOfEnemyKilled;

	// Starting play the next upcoming round
	void PlayRound();
	// Gets an actor by Tag, be aware can return null actor
	AActor* GetActorByTag(FString& Tag);
	// The round has ended prepare for the next round
	void EndRound();
	// The player finished his objective so end the game
	void EndGame();

protected:
	virtual void StartPlay() override;
};