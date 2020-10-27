#include "Enemy/EnemyCharacter.h"
#include "World/RoundBasedGameMode.h"

AEnemyCharacter::AEnemyCharacter()
{}

void AEnemyCharacter::DoDamage(const int& hit, const int& damage) {
	if (hit >= AC) {
		HP -= damage;
		if (HP <= 0) {
			ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
			FString tag = Tags[1].ToString();
			FString sortTag = Tags[0].ToString();
			GameMode->DeleteRound(tag);
			GameMode->UpdateEnemyObjective(sortTag);
			Destroy();
		}
	}
}