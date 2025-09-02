
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetAttackCooldown.generated.h"

UCLASS()
class ARRANGER_API UBTT_SetAttackCooldown : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTT_SetAttackCooldown();

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    float CooldownTime = 5.0f;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
