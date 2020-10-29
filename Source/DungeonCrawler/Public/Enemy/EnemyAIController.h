#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Base/ControllerInterface.h"
#include "EnemyAIController.generated.h"

DECLARE_EVENT(AEnemyAIController, FOnFinishAIRound)

UCLASS()
class DUNGEONCRAWLER_API AEnemyAIController : public AAIController, public IControllerInterface
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
	bool TargetInRange = false;

	int32 AttackToWorldMargin = 130;

	void EndRound();

protected:
	// Begin round when event is being called and it is your turn
	virtual void BeginRound(const FString& name) override;
	virtual void Move() override;
	virtual void Attack() override;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
};