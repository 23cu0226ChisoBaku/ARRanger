// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"

#include "Enemy/EnemyAttackTypes.h"

#include "AnimNotify_AR_AddAttackCondition.generated.h"

#define UE_API ARRANGER_API
/**
 * 
 */
UCLASS(Const, hidecategories=Object, collapsecategories, Config = Game, meta=(DisplayName="Add Attack Condition"), MinimalAPI)
class UAnimNotify_AR_AddAttackCondition : public UAnimNotify
{
	GENERATED_BODY()

protected:
  /**Start UAnimNotify interface */
  UE_API virtual FString GetNotifyName_Implementation() const override;
  UE_API virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	/**End UAnimNotify interface */
	
  private:
    UPROPERTY(EditAnywhere)
    EAttackType ConditionType = EAttackType::None;
};

#undef UE_API