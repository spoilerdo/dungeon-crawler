#include "base/ControllerInterface.h"

bool IControllerInterface::CalcDistance(const APawn* MyPawn) {
	if (MyPawn) {
		Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());
		return true;
	}

	return false;
}