
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy_Zako.generated.h"

UCLASS()
class ARRANGER_API AEnemy_Zako : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy_Zako();

public:
	void SetIsChasing(bool bChasing);

protected:
	// 最大HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int maxHP;

public:

	// 現在のHP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	int currentHP;

	// 死亡フラグ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool isDead;

	// ダメージを受ける関数
	UFUNCTION()
	void ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop);

};
