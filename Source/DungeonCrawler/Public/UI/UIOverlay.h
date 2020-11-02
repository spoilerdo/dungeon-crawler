#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIOverlay.generated.h"

/**
 * UI overlay base class with custom events
 */
UCLASS(Abstract)
class DUNGEONCRAWLER_API UUIOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUIOverlay(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "UI Overlay events")
	void OnAttacked();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Overlay events")
	void OnShowEndGamePanel();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Overlay events")
	void OnChangePhase();
};
