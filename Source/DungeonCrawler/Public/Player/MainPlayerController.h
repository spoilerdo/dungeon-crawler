#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Base/ControllerInterface.h"
#include "Enemy/EnemyCharacter.h"
#include "UI/UIOverlay.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "MainPlayerController.generated.h"

DECLARE_EVENT(AMainPlayerController, FOnFinishRound)

/**
 * Main player controller with an move and attack system.
 * It is also attached to a round based system gamemode.
 */
UCLASS()
class DUNGEONCRAWLER_API AMainPlayerController : public APlayerController, public IControllerInterface
{
	GENERATED_BODY()

public:
	AMainPlayerController();
/**
 * Moving system related variables and methods
 */
	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 Speed;
	float SpeedLeft;

protected:
	// Navigate player to the given world location
	virtual void Move() override;

private:
	int32 SpeedToWorldMargin = 50;

	// Navigate player to the current mouse cursor location. Activates when mouse button is double pressed
	void MoveToMouseCursor();
	// Displays the speed that is left for the user to use
	void DisplaySpeedLeft();

/**
 * Attack system related variables and methods
 */
public:
	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 AttackRange;
	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 Damage;

protected:
	UFUNCTION(BlueprintCallable, Category = "AttackSystem")
	virtual void Attack() override;

private:
	int32 AttackToWorldMargin = 130;
	AEnemyCharacter* AttackGoal;

	// Set attack goal (Enemy object only), is needed before pressing the attack button
	void SetAttackGoal();
	// Set render custom depth for outlining/ highlighting the enemy
	void UpdateRenderCustomDepth(const bool& DepthValue);

/**
 * Round based system related variables and methods
 */
public:
	FOnFinishRound FinishRound;
	FString PlayerName = "P1";
	char CurrentAction;

	// Go to the next phase or use it to skip a method
	UFUNCTION(BlueprintCallable, Category = "PhaseSystem")
	void NextPhase();

	UUIOverlay* UIOverlay;

	bool InputDisabled = false;
	void DisableController(class APlayerController* PlayerController);

private:
	TSubclassOf<UUserWidget> UIOverlayTClass;

	void DisplayCurrentPhase(const FString& Phase);

protected:
	// Begin round when event is being called and it is your turn
	virtual void BeginRound(const FString& name) override;

/**
 * Player controller inherited methods
 */
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
};