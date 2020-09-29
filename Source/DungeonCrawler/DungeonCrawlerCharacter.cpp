// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCrawlerCharacter.h"
#include "DungeonCrawlerPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

ADungeonCrawlerCharacter::ADungeonCrawlerCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	CursorToWorld->SetDecalMaterial(DecalMaterial);
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ADungeonCrawlerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Zoom", this, &ADungeonCrawlerCharacter::Zoom);
	InputComponent->BindAxis("CameraYaw", this, &ADungeonCrawlerCharacter::YawCamera);
}

void ADungeonCrawlerCharacter::Zoom(float AxisValue) {
	ZoomFactor = AxisValue;
}

void ADungeonCrawlerCharacter::YawCamera(float AxisValue) {
	CameraInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void ADungeonCrawlerCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	//Blend our camera's FOV and our SpringArm's length based on ZoomFactor
	CalcZoom();
	//Rotate our actor's yaw, which will turn our camera because we're attached to it
	CalcYaw();

	if (CursorToWorld != nullptr)
	{
		if (ADungeonCrawlerPlayerController* PC = Cast<ADungeonCrawlerPlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
			if (PC->CurrentAction == 'M') {
				CalculateDecal(PC->Speed); 
			}
			else {
				CalculateDecal(PC->AttackRange);
			}
		}
	}
}

void ADungeonCrawlerCharacter::CalculateDecal(int32 MaxDistance) {
	float const Distance = FVector::Dist(CursorToWorld->GetComponentLocation(), GetActorLocation());
	if (Distance > MaxDistance && CursorToWorld->GetDecalMaterial() == DecalMaterial) {
		CursorToWorld->SetDecalMaterial(DecalMaterialUnavailable);
	}
	else if (Distance < MaxDistance && CursorToWorld->GetDecalMaterial() == DecalMaterialUnavailable) {
		CursorToWorld->SetDecalMaterial(DecalMaterial);
	}
}

void ADungeonCrawlerCharacter::CalcZoom() {
	ZoomFactor *= 2;
	float NewFOV = TopDownCameraComponent->FieldOfView += ZoomFactor / 2;
	TopDownCameraComponent->FieldOfView = FMath::Clamp<float>(NewFOV, 60.0f, 90.0f);

	float NewArmLength = CameraBoom->TargetArmLength += ZoomFactor;
	CameraBoom->TargetArmLength = FMath::Clamp<float>(NewArmLength, 600.0f, 800.0f);
}

void ADungeonCrawlerCharacter::CalcYaw() {
	FRotator NewRotation = CameraBoom->GetComponentRotation();
	NewRotation.Yaw += CameraInput.X;
	CameraBoom->SetWorldRotation(NewRotation);
}