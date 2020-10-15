#include "Player/MainPlayerCharacter.h"
#include "Player/MainPlayerController.h"
#include "World/RoundBasedGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AMainPlayerCharacter::AMainPlayerCharacter()
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

void AMainPlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	MaxHp = HP;
	DisplayAC();
}

void AMainPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Zoom", this, &AMainPlayerCharacter::Zoom);
	InputComponent->BindAxis("CameraYaw", this, &AMainPlayerCharacter::YawCamera);
}

void AMainPlayerCharacter::Zoom(float AxisValue) {
	ZoomFactor = AxisValue;
}

void AMainPlayerCharacter::YawCamera(float AxisValue) {
	CameraInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AMainPlayerCharacter::Tick(float DeltaSeconds) {
    Super::Tick(DeltaSeconds);

	//Blend our camera's FOV and our SpringArm's length based on ZoomFactor
	CalcZoom();
	//Rotate our actor's yaw, which will turn our camera because we're attached to it
	CalcYaw();

	if (CursorToWorld != nullptr)
	{
		if (AMainPlayerController* PC = Cast<AMainPlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
			CalculateDecal(PC->SpeedLeft);
		}
	}
}

void AMainPlayerCharacter::CalculateDecal(float MaxDistance) {
	float const Distance = FVector::Dist(CursorToWorld->GetComponentLocation(), GetActorLocation());
	if (Distance > MaxDistance && CursorToWorld->GetDecalMaterial() == DecalMaterial) {
		CursorToWorld->SetDecalMaterial(DecalMaterialUnavailable);
	}
	else if (Distance < MaxDistance && CursorToWorld->GetDecalMaterial() == DecalMaterialUnavailable) {
		CursorToWorld->SetDecalMaterial(DecalMaterial);
	}
}

void AMainPlayerCharacter::CalcZoom() {
	ZoomFactor *= 2;
	float NewFOV = TopDownCameraComponent->FieldOfView += ZoomFactor / 2;
	TopDownCameraComponent->FieldOfView = FMath::Clamp<float>(NewFOV, 60.0f, 90.0f);

	float NewArmLength = CameraBoom->TargetArmLength += ZoomFactor;
	CameraBoom->TargetArmLength = FMath::Clamp<float>(NewArmLength, 600.0f, 800.0f);
}

void AMainPlayerCharacter::CalcYaw() {
	FRotator NewRotation = CameraBoom->GetComponentRotation();
	NewRotation.Yaw += CameraInput.X;
	CameraBoom->SetWorldRotation(NewRotation);
}

void AMainPlayerCharacter::DoDamage(int hit, int damage) {
	if (hit >= AC) {
		HP -= damage;
		DisplayHP();
		if (HP <= 0) {
			ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
			GameMode->DeleteRound(Tags[0].ToString());
			Destroy();
		}
	}
}

void AMainPlayerCharacter::DisplayHP() {
	if (AMainPlayerController* PC = Cast<AMainPlayerController>(GetController())) {
		UWidget* bar = PC->UIOverlay->GetWidgetFromName("HPBar");
		float progress = (float)HP / (float)MaxHp;
		if (bar != NULL) Cast<UProgressBar>(bar)->SetPercent(progress);
	}
}

void AMainPlayerCharacter::DisplayAC() {
	if (AMainPlayerController* PC = Cast<AMainPlayerController>(GetController())) {
		UWidget* text = PC->UIOverlay->GetWidgetFromName("ACText");
		FString ACText = FString::FromInt(AC) + "\nAC";
		if (text != NULL) Cast<UTextBlock>(text)->SetText(FText::FromString(ACText));
	}
}