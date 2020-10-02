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

	void DoDamage(int hit, int damage);
};
