
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetRandomPatrolLocation.generated.h"

UCLASS()
class ARRANGER_API UBTT_SetRandomPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_SetRandomPatrolLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    // œpœjæ‚ğ•Û‘¶‚·‚éƒL[ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector PatrolLocationKey;

    // œpœj‚ÌŠî€’n“_‚ğ•Û‘¶‚·‚éƒL[ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector PatrolOriginKey;

    // œpœj”ÍˆÍ‚Ì”¼Œa 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    float SearchRadius = 300.0f;
};