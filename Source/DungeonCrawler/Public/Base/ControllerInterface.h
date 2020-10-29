#pragma once

#include "CoreMinimal.h"

#include "ControllerInterface.generated.h"

/**
 * 
 */
UINTERFACE()
class DUNGEONCRAWLER_API UControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONCRAWLER_API IControllerInterface
{
	GENERATED_BODY()

public:
	float Distance;
	FVector DestLocation;
	
	virtual void BeginRound(const FString& name) = 0;
	virtual void Move() = 0;
	// Calculates distance and returns true if successful
	bool CalcDistance(const APawn* MyPawn);
	virtual void Attack() = 0;
};
