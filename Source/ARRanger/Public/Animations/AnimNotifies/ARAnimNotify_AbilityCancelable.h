// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "ARAnimNotify_AbilityCancelable.generated.h"

#define UE_API ARRANGER_API

UENUM(BlueprintType)
enum struct EARAbilityCancelType : uint8
{
  Cancel UMETA(DisplayName = "Cancel Ability"), // アビリティをキャンセルできるようにする
  Block  UMETA(DisplayName = "Block Cancel"),   // アビリティキャンセルをブロックする

  TYPE_MAX UMETA(Hidden)
};

/**
 * 
 */
UCLASS(Const, hidecategories=Object, collapsecategories, Config = Game, meta=(DisplayName="Set Ability Cancelable"), MinimalAPI)
class UARAnimNotify_AbilityCancelable : public UAnimNotify
{
	GENERATED_BODY()

public:

  UE_API UARAnimNotify_AbilityCancelable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  /**Start UAnimNotify interface */
  UE_API virtual FString GetNotifyName_Implementation() const override;
  UE_API virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	/**End UAnimNotify interface */

private:

  UPROPERTY(EditAnywhere, Category = "Cancel Parameter", meta = (DisplayName = "Notify Type", AllowPrivateAccess = "true"))
  EARAbilityCancelType CancelProcessType;
};

#undef UE_API
