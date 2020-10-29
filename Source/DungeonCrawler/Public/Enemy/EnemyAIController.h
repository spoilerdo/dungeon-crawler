#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Base/ControllerInterface.h"
#include "EnemyAIController.generated.h"

DECLARE_EVENT(AEnemyAIController, FOnFinishAIRound)

/**
 * Enemy AI Controller with an moving and attack system.
 * It is also attached to a round based system gamemode.
 */
UCLASS()
class DUNGEONCRAWLER_API AEnemyAIController : public AAIController, public IControllerInterface
{
	GENERATED_BODY()

public:
	AEnemyAIController();
/**
 * Moving system related variables and methods
 */
	UPROPERTY(EditAnywhere, Category = "EnemyStats", BlueprintReadWrite)
	int32 Speed;

protected:
	// Move to the main player (target)
	virtual void Move() override;

private:
	int32 SpeedToWorldMargin = 50;

/**
 * Attack system related variables and methods
 */
public:
	UPROPERTY(EditAnywhere, Category = "EnemyStats", BlueprintReadWrite)
	int32 AttackRange;
	UPROPERTY(EditAnywhere, Category = "EnemyStats", BlueprintReadWrite)
	int32 Damage;

protected:
	virtual void Attack() override;

private:
	int32 AttackToWorldMargin = 130;

/**
 * Round based system related variables and methods
 */
public:
	FOnFinishAIRound FinishRound;
	FString Name;
	bool MyTurn = false;

private:
	bool TargetInRange = false;

	void EndRound();

protected:
	// Begin round when event is being called and it is your turn
	virtual void BeginRound(const FString& name) override;

/**
 * AI controller inherited methods
 */
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
};