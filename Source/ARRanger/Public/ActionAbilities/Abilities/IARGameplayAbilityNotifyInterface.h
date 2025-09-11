// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARGameplayAbilityNotifyInterface.generated.h"

#define UE_API ARRANGER_API

/**
 * 
 */
struct FGANotify_ImpactResult
{
  TObjectPtr<AActor> SourceActor;
  TObjectPtr<AActor> HitActor;
  TObjectPtr<USceneComponent> OccurrenceComp;
  FVector ImpactLocation;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARGameplayAbilityNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IARGameplayAbilityNotifyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

  UE_API virtual void GANotify_ActorArray(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const TArray<TObjectPtr<AActor>>& InActorArray) { }
  UE_API virtual void GANotify_ImpactResult(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const TArray<FGANotify_ImpactResult>& InImpactResults) { }
};

#undef UE_API
