
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAttackTypes.h" // ← EAttackType列挙型を定義しているファイル
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SelectAttack.generated.h"

UCLASS()
class ARRANGER_API UBTT_SelectAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_SelectAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// ランダム選択に使う攻撃リスト
	UPROPERTY(EditAnywhere, Category = "Attack")
	TArray<EAttackType> CandidateAttacks;

	// 重み付き選択に使う場合
	UPROPERTY(EditAnywhere, Category = "Attack")
	TMap<EAttackType, float> WeightedAttacks;
};
