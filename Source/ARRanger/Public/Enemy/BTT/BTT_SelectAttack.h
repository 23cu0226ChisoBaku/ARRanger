
#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAttackTypes.h" // �� EAttackType�񋓌^���`���Ă���t�@�C��
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

	UPROPERTY(EditAnywhere, Category = "Attack")
	TArray<EAttackType> CandidateAttacks;


	UPROPERTY(EditAnywhere, Category = "Attack")
	TMap<EAttackType, float> WeightedAttacks;
};
