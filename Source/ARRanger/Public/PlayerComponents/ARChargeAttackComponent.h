// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "GameplayTagContainer.h"

#include "ARChargeAttackComponent.generated.h"

struct FGameplayTag;

#define UE_API ARRANGER_API

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UARChargeAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UE_API UARChargeAttackComponent();

protected:

  /**Start AActor Interface */
	UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  /**End AActor Interface */

public:	
	// Called every frame
	UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UE_API void AddChargeAttack(const FGameplayTag& InBranchTag, float InChargeTimeThreshold, float InChargeAttackCost, const FGameplayTag& InChargeAttackTag);

  UE_API FGameplayTag EvaluateCharge(float InTimeCharged, const FGameplayTag& InBranchTag) const;

  UE_API void ClearAllChargeAttacks();

  // TODO Temporary
  UE_API FGameplayTag GetAvailableAttackTagByHeldTime(float InTimeHeld, const FGameplayTag& InBranchTag) const;

  UE_API float GetChargeTimeThresholdByAttackTag(const FGameplayTag& InChargeAttackTag) const;
  UE_API float GetChargeAttackCostByAttackTag(const FGameplayTag& InChargeAttackTag) const;

  UE_API float GetNextChargeTimeThresholdByAttackTag(const FGameplayTag& InChargeAttackTag) const;
  UE_API float GetNextChargeAttackCostByAttackTag(const FGameplayTag& InChargeAttackTag) const;

private:
  struct FChargeAttackInfo
  {
    float TimeThreshold;

    float AttackCost;

    FGameplayTag ChargeAttackTag;

    bool operator==(const FChargeAttackInfo& Other) const;
  };

  TMap< FGameplayTag, TArray< FChargeAttackInfo > > m_chargeAttackTrees;
		
};


#undef UE_API