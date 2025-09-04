// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/ARAnimNotify_AbilityCancelable.h"

UARAnimNotify_AbilityCancelable::UARAnimNotify_AbilityCancelable(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , CancelProcessType(EARAbilityCancelType::Cancel)
{ }

FString UARAnimNotify_AbilityCancelable::GetNotifyName_Implementation() const
{
  return StaticEnum<EARAbilityCancelType>()->GetValueAsString(CancelProcessType);
}

void UARAnimNotify_AbilityCancelable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
  Super::Notify(MeshComp, Animation, EventReference);


}