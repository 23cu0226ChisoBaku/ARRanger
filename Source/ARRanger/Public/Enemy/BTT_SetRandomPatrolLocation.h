// Fill out your copyright notice in the Description page of Project Settings.

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float SearchRadius = 500.0f;
};
