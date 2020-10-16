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
	FString EnemyObjectiveTag;
	int16 AmountOfEnemyObjectives;
	FVector Destination;

	// Delete a player or enemy from the round
	void DeleteRound(FString& Tag);
private:
	int16 Index;

	// Starting play the next upcoming round
	void PlayRound();
	// The round has ended prepare for the next round
	void EndRound();
	void CheckObjective();
	void UpdateObjective(FString& EnemyTag, int16& NewEnemyObjective, FVector& NewDestination);

protected:
	virtual void StartPlay() override;
};