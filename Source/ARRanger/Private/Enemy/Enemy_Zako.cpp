
#include "Enemy/Enemy_Zako.h"
#include "Enemy/EnemyAnimInstance.h"
#include "InsekiGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/ZakoAIController.h"

AEnemy_Zako::AEnemy_Zako()
: maxHP(100)
, currentHP(maxHP)
, isDead(false)
{
    // AIController の指定（C++でもできる）
    AIControllerClass = AZakoAIController::StaticClass();

    // プレイヤーではないので自動制御をAIに
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemy_Zako::SetIsChasing(bool bChasing)
{
    if (UEnemyAnimInstance* Anim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        Anim->bIsChasing = bChasing;
    }
}

void AEnemy_Zako::ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop)
{
	currentHP -= DamageAmount;

	if (currentHP <= 0)
	{
		// 死亡フラグを上げる
		isDead = true;

		// 最後の一撃！強く吹っ飛ばす
		LaunchCharacter(LaunchDirection * 1500.f, true, true);

		// GameModeに通知
		if (AInsekiGameMode* GM = Cast<AInsekiGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->OnEnemyKilled();
		}
		// ちょっと待ってから消す
		SetLifeSpan(1.0f);
	}
	else
	{
		// 途中の攻撃。軽く吹っ飛ばすだけ
		LaunchCharacter(LaunchDirection * 600.f, true, true);
	}

	// ヒットストップ：とどめの一撃だけ有効化
	if (bEnableHitStop)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, []()
			{
				UGameplayStatics::SetGlobalTimeDilation(GWorld, 1.0f);
			}, 0.03f, false);
	}
}