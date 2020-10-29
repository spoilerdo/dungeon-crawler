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
 * Main player controller
 */
UCLASS()
class DUNGEONCRAWLER_API AMainPlayerController : public APlayerController, public IControllerInterface
{
	GENERATED_BODY()

public:
	AMainPlayerController();

	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 Speed;
	float SpeedLeft;
	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 AttackRange;
	UPROPERTY(EditAnywhere, Category = "PlayerStats", BlueprintReadWrite)
	int32 Damage;

	// Round based system variables
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
	int32 SpeedToWorldMargin = 50;

	int32 AttackToWorldMargin = 130;
	AEnemyCharacter* AttackGoal;

	TSubclassOf<UUserWidget> UIOverlayTClass;

	// Navigate player to the current mouse cursor location.
	void MoveToMouseCursor();
	// Displays the speed that is left for the user to use
	void DisplaySpeedLeft();
	// Set the attack goal of the character
	void SetAttackGoal();
	// Set render custom depth for outlining the enemy
	void UpdateRenderCustomDepth(const bool& DepthValue);
	// Displays the current phase of the player
	void DisplayCurrentPhase(const FString& Phase);

protected:
	// Begin round when event is being called and it is your turn
	virtual void BeginRound(const FString& name) override;
	// Navigate player to the given world location
	virtual void Move() override;
	UFUNCTION(BlueprintCallable, Category = "AttackSystem")
	virtual void Attack() override;

	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
};