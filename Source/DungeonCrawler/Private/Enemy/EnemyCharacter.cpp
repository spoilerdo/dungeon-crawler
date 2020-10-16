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
			GameMode->DeleteRound(tag);
			Destroy();
		}
	}
}