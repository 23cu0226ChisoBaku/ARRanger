#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/EnemyAttackTypes.h" // EAttackType 定義
#include "BTT_PerformAttack.generated.h"

class ACharacter;
class UAnimMontage;
class UBlackboardComponent;

UCLASS()
class ARRANGER_API UBTT_PerformAttack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTT_PerformAttack();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
    EBTNodeResult::Type PlayAttackMontage(ACharacter* Boss, UAnimMontage* Montage, EAttackType AttackType);
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    void PerformAttackEffect(ACharacter* Boss, EAttackType AttackType);

protected:
    UPROPERTY(EditAnywhere, Category = "Attack")
    float PunchRange = 200.f;

    UPROPERTY(EditAnywhere, Category = "Attack")
    float PunchMoveSpeed = 300.f;

    UPROPERTY(EditAnywhere, Category = "Attack")
    float JumpMoveSpeed = 600.f;

    UPROPERTY(EditAnywhere, Category = "Attack")
    UAnimMontage* PunchMontage;

    UPROPERTY(EditAnywhere, Category = "Attack")
    UAnimMontage* JumpAttackMontage;

    UPROPERTY(EditAnywhere, Category = "Attack")
    UAnimMontage* RoarMontage;

    UPROPERTY()
    UBlackboardComponent* CachedBB;

    UPROPERTY()
    class AAIController* CachedAICon;

    UPROPERTY()
    UBehaviorTreeComponent* CachedOwnerComp;

    UPROPERTY()
    AActor* TargetActor;

    bool bIsMovingToTarget = false;

    // ジャンプ攻撃用
    bool bHasStartedJump = false;
    FVector JumpTargetLocation;
};
