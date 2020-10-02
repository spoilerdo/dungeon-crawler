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

	// Go to the next phase or use it to skip a method
	UFUNCTION(BlueprintCallable, Category = "PhaseSystem")
	void NextPhase();

private:
	int32 SpeedToWorldMargin = 50;
	FVector DestLocation;
	float Distance;

	int32 AttackToWorldMargin = 130;
	AEnemyCharacter* AttackGoal;

	// Begin round when event is being called and it is your turn
	void BeginRound(FString name);
	// Navigate player to the current mouse cursor location.
	void MoveToMouseCursor();
	// Navigate player to the given world location
	void Move();
	// Calculates distance and returns true if succesfull
	bool CalcDistance();
	// Set the attack goal of the character
	void SetAttackGoal();
	// Set render custom depth for outlining the enemy
	void UpdateRenderCustomDepth(bool DepthValue);

protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
};


