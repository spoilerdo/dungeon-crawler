// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DungeonCrawlerGameMode.generated.h"

DECLARE_EVENT_OneParam(ADungeonCrawlerGameMode, FOnActivateRound, FString);

UCLASS(minimalapi)
class ADungeonCrawlerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADungeonCrawlerGameMode();

	FOnActivateRound ActivateRound;

	UPROPERTY(EditAnywhere, Category = "Round base stats")
	TArray<FString> rounds;
private:
	void PlayRound();
	void EndRound();

protected:
	virtual void StartPlay() override;
};



