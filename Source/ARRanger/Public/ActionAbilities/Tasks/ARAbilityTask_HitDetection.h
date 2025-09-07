// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "ARAbilityTask_HitDetection.generated.h"

#define UE_API ARRANGER_API

class UPrimitiveDetectorData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHitDetectionTask);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitDetectionDelegate, const TArray<AActor*>&, InHitActors);

USTRUCT(BlueprintType, Blueprintable)
struct FARHitDetectionParameter
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability|HitDetection", meta = (DisplayName = "Hit Detection Range Data"))
  TObjectPtr<UPrimitiveDetectorData> HitRange;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability|HitDetection", meta = (DisplayName = "Detect Duration"))
  float Duration;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability|HitDetection", meta = (DisplayName = "End On Hit Detected"))
  bool bEndIfHit;
};

UCLASS(MinimalAPI)
class UARAbilityTask_HitDetection : public UAbilityTask
{
	GENERATED_BODY()

  enum ETypeInternal
  {
    Internal_NotInit,
    Internal_Actor,
    Internal_Interface,
  };

public:

  UE_API UARAbilityTask_HitDetection(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:  
  UPROPERTY(BlueprintAssignable)
  FHitDetectionTask OnHitDetectionFinish;

  UPROPERTY(BlueprintAssignable)
  FHitDetectionDelegate OnHitDetected;
	
	UE_API virtual void InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UE_API UARAbilityTask_HitDetection* HitDetection_Actor(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FARHitDetectionParameter& InHitDetectionParam, TSubclassOf<AActor> TargetActorClass);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UE_API UARAbilityTask_HitDetection* HitDetection_Interface(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FARHitDetectionParameter& InHitDetectionParam, TSubclassOf<UInterface> TargetInterfaceClass);

	UE_API virtual void Activate() override;

	/** Tick function for this task, if bTickingTask == true */
	UE_API virtual void TickTask(float DeltaTime) override;

	UE_API virtual void OnDestroy(bool AbilityIsEnding) override;

protected:
  UE_API virtual void OnHit(const TArray<TObjectPtr<AActor>>& InHitResult) {}

private:
  void EndHitDetectionTask();
  bool CanDetectHit() const;

private:

  UPROPERTY()
  TObjectPtr<const UPrimitiveDetectorData> HitRange;

  TSubclassOf<UObject> m_detectionTargetClass;

  float m_duration = 0.0f;
  float m_timeDetectionWillEnd;

  ETypeInternal m_type = Internal_NotInit;

  uint8 bEndIfHit : 1;
};

#undef UE_API