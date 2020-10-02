#include "Enemy/EnemyCharacter.h"

AEnemyCharacter::AEnemyCharacter()
{}

void AEnemyCharacter::DoDamage(int hit, int damage) {
	if (hit >= AC) {
		HP -= damage;
		if (HP <= 0) {
			Destroy();
		}
	}
}