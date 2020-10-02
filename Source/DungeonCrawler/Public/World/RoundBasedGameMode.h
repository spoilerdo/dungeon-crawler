#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RoundBasedGameMode.generated.h"

DECLARE_EVENT_OneParam(ARoundBasedGameMode, FOnActivateRound, FString);

UCLASS(minimalapi)
class DUNGEONCRAWLER_API ARoundBasedGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARoundBasedGameMode();

	// Round based system variables
	FOnActivateRound ActivateRound;
	UPROPERTY(EditAnywhere, Category = "Round base stats")
	TArray<FString> rounds;
private:
	int16 index;

	// Starting play the next upcoming round
	void PlayRound();
	// The round has ended prepare for the next round
	void EndRound();

protected:
	virtual void StartPlay() override;
};