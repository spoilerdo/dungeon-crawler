// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DungeonCrawlerPlayerController.generated.h"

DECLARE_EVENT(ADungeonCrawlerPlayerController, FOnFinishRound)

UCLASS()
class ADungeonCrawlerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADungeonCrawlerPlayerController();

	FOnFinishRound FinishRound;

	FString PlayerName = "P1";
	
	UPROPERTY(EditAnywhere, Category="PlayerStats", BlueprintReadWrite)
	int32 Speed;

	int32 MaxDistance;

private:
	int32 SpeedToWorldMargin = 50;
	bool IsYourRound = true;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	void BeginRound(FString name);

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
};


