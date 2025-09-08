// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifies/ARAnimNotify_AbilityCancelable.h"

#include "ActionAbilities/ARAbilitySystemComponent.h"

UARAnimNotify_AbilityCancelable::UARAnimNotify_AbilityCancelable(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , CancelProcessType(EARAbilityCancelType::Cancel)
{ }

FString UARAnimNotify_AbilityCancelable::GetNotifyName_Implementation() const
{
  return StaticEnum<EARAbilityCancelType>()->GetDisplayValueAsText(CancelProcessType).ToString();
}

void UARAnimNotify_AbilityCancelable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
  Super::Notify(MeshComp, Animation, EventReference);

  AActor* ownerActor = MeshComp->GetOwner();

  UARAbilitySystemComponent* AR_ASC = UARAbilitySystemComponent::FindARAbilitySystemComponent(ownerActor);
  if (AR_ASC != nullptr)
  {
    // TODO Notify to Set cancel state of ability
    using enum EARAbilityCancelType;
    switch (CancelProcessType)
    {
      case Cancel:
      {
        AR_ASC->NotifyAbilityCancelable();
      }
      break;

      case Block:
      {
        AR_ASC->NotifyAbilityBlock();
      }
      break;

      default:
      {
        // Not implemented
        checkSlow(false);
      }
      break;
    }
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Can not find ASC."));
  }
}