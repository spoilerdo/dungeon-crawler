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

	UPROPERTY(EditAnywhere, Category = "EnemyStats", BlueprintReadWrite)
	int32 Speed;
	UPROPERTY(EditAnywhere, Category = "EnemyStats", BlueprintReadWrite)
	int32 AttackRange;
	UPROPERTY(EditAnywhere, Category = "EnemyStats", BlueprintReadWrite)
	int32 Damage;

	// Round based system variables
	FOnFinishAIRound FinishRound;
	FString Name;
	bool MyTurn = false;
private:
	int32 SpeedToWorldMargin = 50;
	FVector DestLocation;
	float Distance;
	bool TargetInRange = false;

	int32 AttackToWorldMargin = 130;

	// Begin round when event is being called and it is your turn
	void BeginRound(const FString& name);
	void Move();
	// Calculates distance and returns true if successful
	bool CalcDistance();
	void Attack();
	void EndRound();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
};