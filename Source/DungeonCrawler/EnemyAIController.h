// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

DECLARE_EVENT(AEnemyAIController, FOnFinishAIRound)

UCLASS()
class DUNGEONCRAWLER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

	// Round based system variables
	FOnFinishAIRound FinishRound;
	FString Name = "E1";
private:
	FVector DestLocation;
	float Distance;

	bool BeginMoving = false;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	void BeginRound(FString name);

	// Calculates distance and returns true if sucessfull
	bool CalcDistance();
};
