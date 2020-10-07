#pragma once

#include "CoreMinimal.h"
#include "Player/MainPlayerController.h"
#include "GameFramework/Character.h"
#include "MainPlayerCharacter.generated.h"

UCLASS(Blueprintable)
class DUNGEONCRAWLER_API AMainPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainPlayerCharacter();

	UPROPERTY(EditAnywhere, Category = "Decals", BlueprintReadWrite)
	UMaterial* DecalMaterial;
	UPROPERTY(EditAnywhere, Category = "Decals", BlueprintReadWrite)
	UMaterial* DecalMaterialUnavailable;
	
	// Health Points
	UPROPERTY(EditAnywhere, Category = "Character stats", BlueprintReadWrite)
	int HP;
	// Armor Combat
	UPROPERTY(EditAnywhere, Category = "Character stats", BlueprintReadWrite)
	int AC;

	void DoDamage(int hit, int damage);

	// Returns TopDownCameraComponent subobject
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	// Returns CameraBoom subobject
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Returns CursorToWorld subobject
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	// Top down camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;
	// Camera boom positioning the camera above the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	// A decal that projects to the cursor location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	// Input variables
	FVector2D CameraInput;
	float ZoomFactor;

	void CalculateDecal(int32 MaxDistance);

	// Input functions
	void Zoom(float AxisValue);
	void CalcZoom();
	void YawCamera(float AxisValue);
	void CalcYaw();

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
};