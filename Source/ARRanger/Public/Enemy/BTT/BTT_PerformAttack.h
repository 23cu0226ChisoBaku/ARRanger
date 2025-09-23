#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/EnemyAttackTypes.h" 
#include "BTT_PerformAttack.generated.h"

class ACharacter;
class AAIController;
class UAnimMontage;
class UBlackboardComponent;
class AEnemy_MiddleBoss;

#define UE_API ARRANGER_API

UCLASS()
class UBTT_PerformAttack : public UBTTaskNode
{
    GENERATED_BODY()

public:
  UE_API UBTT_PerformAttack();

  UE_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
  UE_API virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:

  UE_API void OnPreAttackFinished(EAttackType InType);

  UFUNCTION()
  UE_API void OnAttackFinished();

protected:

  UPROPERTY()
  TObjectPtr<UBlackboardComponent> CachedBB;

  UPROPERTY()
  TObjectPtr<AAIController> CachedAICon;

  UPROPERTY()
  TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

  UPROPERTY()
  TObjectPtr<AEnemy_MiddleBoss> Boss;
};

#undef UE_API