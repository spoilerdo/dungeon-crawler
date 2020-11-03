#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class DUNGEONCRAWLER_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

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
};