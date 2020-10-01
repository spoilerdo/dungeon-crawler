// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnemyCharacter.h"
#include "DungeonCrawlerPlayerController.generated.h"

DECLARE_EVENT(ADungeonCrawlerPlayerController, FOnFinishRound)

UCLASS()
class ADungeonCrawlerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADungeonCrawlerPlayerController();

	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 Speed;
	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 AttackRange;

	// Round based system variables
	FOnFinishRound FinishRound;
	FString PlayerName = "P1";

	char CurrentAction;

	// Attack the goal
	UFUNCTION(BlueprintCallable, Category = "AttackSystem")
	void Attack();

private:
	int32 SpeedToWorldMargin = 50;
	FVector DestLocation;
	float Distance;

	bool IsYourRound = true;

	int32 AttackToWorldMargin = 130;
	AEnemyCharacter* AttackGoal;

protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void BeginRound(FString name);

	// Navigate player to the current mouse cursor location.
	void MoveToMouseCursor();
	// Navigate player to the given world location
	void SetNewMoveDestination();
	// Calculates distance and returns true if succesfull
	bool CalcDistance();
	// Set the attack goal of the character
	void SetAttackGoal();
	// Set render custom depth for outlining the enemy
	void UpdateRenderCustomDepth(bool DepthValue);

	void EndRound();
};


