// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "ARAbilityTask_HitDetection.generated.h"

#define UE_API ARRANGER_API

class UPrimitiveDetectorData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitDetectionDelegate, const TArray<TObjectPtr<AActor>>&);

USTRUCT(BlueprintType)
struct FARHitDetectionParameter
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Ability|HitDetection", meta = (DisplayName = "Hit Detection Range Data"))
  TObjectPtr<UPrimitiveDetectorData> HitRange;

  UPROPERTY(EditAnywhere, Category = "Ability|HitDetection", meta = (DisplayName = "Detect Duration"))
  float Duration;
};

UCLASS(MininalAPI)
class UARAbilityTask_HitDetection : public UAbilityTask
{
	GENERATED_BODY()

public:

  UE_API UARAbilityTask_HitDetection(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:  
  UPROPERTY(BlueprintAssignable)
  FHitDetectionDelegate OnHitDetectedDelegate;
	
	UE_API virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UE_API UARAbilityTask_HitDetection* HitDetection(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FARHitDetectionParameter& InHitDetectionParam);

	UE_API virtual void Activate() override;

	/** Tick function for this task, if bTickingTask == true */
	UE_API virtual void TickTask(float DeltaTime) override;

	UE_API virtual void OnDestroy(bool AbilityIsEnding) override;

protected:
  UE_API virtual void OnHitDetected(const TArray<TObjectPtr<AActor>>& InHitResult);

private:

  UPROPERTY()
  TObjectPtr<const UPrimitiveDetectorData> HitRange;

  float m_duration = 0.0f;

  float m_timeDetectionStarted;
  float m_timeDetectionWillEnd;
};

#undef UE_API