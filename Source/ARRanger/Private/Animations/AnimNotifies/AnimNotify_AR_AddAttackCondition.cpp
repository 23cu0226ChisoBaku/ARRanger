// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/AnimNotify_AR_AddAttackCondition.h"

#include "Enemy/Enemy_MiddleBoss.h"

FString UAnimNotify_AR_AddAttackCondition::GetNotifyName_Implementation() const
{
  return FString::Printf(TEXT("Condition Type: [%s]"), *::StaticEnum<EAttackType>()->GetDisplayValueAsText(ConditionType).ToString());
}

void UAnimNotify_AR_AddAttackCondition::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
  Super::Notify(MeshComp, Animation, EventReference);

  // TODO This AnimNotify is specific use
  if (AEnemy_MiddleBoss* MiddleBoss = ::Cast<AEnemy_MiddleBoss>(MeshComp->GetOwner()))
  {
    MiddleBoss->AddAttackTaskCondition(ConditionType);
  }
}