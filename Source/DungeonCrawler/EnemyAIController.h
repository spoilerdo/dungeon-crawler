// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

DECLARE_EVENT(AEnemyAIController, FOnFinishRound)

UCLASS()
class DUNGEONCRAWLER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

	// Round based system variables
	FOnFinishRound FinishRound;
	FString Name = "E1";
private:
	bool IsYourRound = false;

protected:
	virtual void BeginPlay() override;

	void BeginRound(FString name);
};
