
#include "Enemy/BTT_PunchAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

UBTT_PunchAttack::UBTT_PunchAttack()
{
	NodeName = TEXT("Punch Attack");
}

EBTNodeResult::Type UBTT_PunchAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 攻撃対象を取得
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
	if (!AICharacter) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	// 攻撃判定
	// 敵とプレイヤーの距離をチェック
	float Distance = FVector::Dist(AICharacter->GetActorLocation(), TargetActor->GetActorLocation());
	
	if (Distance <= AttackRange)
	{
		// 攻撃がヒットした場合の処理
		// ダメージを与える
		//UGameplayStatics::ApplyDamage(TargetActor, Damage, AIController, AICharacter, UDamageType::StaticClass());
		
		// パーティクルエフェクトとサウンドを再生
		FVector HitLocation = TargetActor->GetActorLocation();
		if (HitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation);
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitLocation);
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}