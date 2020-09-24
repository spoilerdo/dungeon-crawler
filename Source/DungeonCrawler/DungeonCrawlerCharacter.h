// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonCrawlerPlayerController.h"
#include "GameFramework/Character.h"
#include "DungeonCrawlerCharacter.generated.h"

UCLASS(Blueprintable)
class ADungeonCrawlerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADungeonCrawlerCharacter();

	UPROPERTY(EditAnywhere, Category = "Decals", BlueprintReadWrite)
	UMaterial* DecalMaterial;
	UPROPERTY(EditAnywhere, Category = "Decals", BlueprintReadWrite)
	UMaterial* DecalMaterialUnavailable;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:

	void CalculateDecal(int32 MaxDistance);

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	// Input variables
	FVector2D CameraInput;
	float ZoomFactor;

	// Input functions
	void Zoom(float AxisValue);
	void CalcZoom();
	void YawCamera(float AxisValue);
	void CalcYaw();
};

