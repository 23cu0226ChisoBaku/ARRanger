
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetChasing.generated.h"

UCLASS()
class ARRANGER_API UBTT_SetChasing : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTT_SetChasing();

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    bool bChasing;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
    bool bIsSearch;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
