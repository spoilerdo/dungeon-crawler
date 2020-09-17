// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Runtime\AIModule\Classes\Blueprint\AIBlueprintHelperLibrary.h"

AEnemyAIController::AEnemyAIController() {

}

void AEnemyAIController::BeginPlay() {
	
	Super::BeginPlay();

	FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	MoveToLocation(PlayerLocation);

	//Attack system
	//Check if it misses or hits
	//Animation
	//Remove health from player
}