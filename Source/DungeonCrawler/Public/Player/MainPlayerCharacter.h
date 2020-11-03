#pragma once

#include "CoreMinimal.h"
#include "Player/MainPlayerController.h"
#include "GameFramework/Character.h"
#include "MainPlayerCharacter.generated.h"

/**
 * Main player character that contains a third person camera.
 * It has a decal system for the mouse, also a yaw and zoom functionality.
 * Lastly it contains a health system.
 */
UCLASS(Blueprintable)
class DUNGEONCRAWLER_API AMainPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

/**
 * Input related variables and methods
 */
public:
	AMainPlayerCharacter();

	// Returns TopDownCameraComponent subobject
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	// Returns CameraBoom subobject
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Returns CursorToWorld subobject
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	UPROPERTY(EditAnywhere, Category = "Decals", BlueprintReadWrite)
	UMaterial* DecalMaterial;
	UPROPERTY(EditAnywhere, Category = "Decals", BlueprintReadWrite)
	UMaterial* DecalMaterialUnavailable;

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

	void CalculateDecal(float& MaxDistance);

	// Input variables
	FVector2D CameraInput;
	float ZoomFactor;

	// Input functions
	void Zoom(float AxisValue);
	void CalcZoom();
	void YawCamera(float AxisValue);
	void CalcYaw();

/**
 * Health system related variables and methods
 */
public:
	// Health Points
	UPROPERTY(EditAnywhere, Category = "Character stats", BlueprintReadWrite)
	int HP;
	// Armor Combat
	UPROPERTY(EditAnywhere, Category = "Character stats", BlueprintReadWrite)
	int AC;

	UPROPERTY(BlueprintReadOnly, Category = "Damage system")
	bool IsHit;
	UFUNCTION(BlueprintCallable, Category = "Damage system")
	void DoneDamageAnim();

	void DoDamage(const int& hit, const int& damage);

private:
	// Max hp that will be used to calculate the progress for the HP bar
	int MaxHp;

	void DisplayHP();
	void DisplayAC();

/**
 * Player character inherited methods
 */
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	virtual void BeginPlay() override;
};