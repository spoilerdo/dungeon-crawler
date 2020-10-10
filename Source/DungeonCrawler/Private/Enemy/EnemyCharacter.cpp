#include "Enemy/EnemyCharacter.h"
#include "World/RoundBasedGameMode.h"

AEnemyCharacter::AEnemyCharacter()
{}

void AEnemyCharacter::DoDamage(int hit, int damage) {
	if (hit >= AC) {
		HP -= damage;
		if (HP <= 0) {
			ARoundBasedGameMode* GameMode = (ARoundBasedGameMode*)GetWorld()->GetAuthGameMode();
			GameMode->DeleteRound(Tags[1].ToString());
			Destroy();
		}
	}
}