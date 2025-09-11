#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MoveToPreferredDistance.generated.h"

UCLASS()
class ARRANGER_API UBTT_MoveToPreferredDistance : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTT_MoveToPreferredDistance();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    /** ç≈å„Ç…éwé¶ÇµÇΩÉSÅ[Éã */
    FVector CurrentMoveGoal;

    /** à⁄ìÆíÜÇ©Ç«Ç§Ç© */
    bool bIsMoving;
};
