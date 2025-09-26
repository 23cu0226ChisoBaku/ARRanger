// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "ARHealthComponent.generated.h"

#define UE_API ARRANGER_API

class UCurveFloat;

USTRUCT(BlueprintType)
struct FARHealthRegenerationEntry
{
  GENERATED_BODY()

public:

  UPROPERTY(EditDefaultsOnly)
  float RegenerationDelay;

  UPROPERTY(EditDefaultsOnly)
  float RegenerationSpeed;

  UPROPERTY(EditDefaultsOnly)
  bool bUseRegenerationSpeedOverrideCurve;

  UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseRegenerationSpeedOverrideCurve == true", EditConditionHides))
  TSoftObjectPtr<UCurveFloat> OverrideCurve;

  float RegenerationCurveEvaluationTimeValue = 0.0f;

  float RegenerationDelayTimeCnt = 0.0f;
  
  uint8 bEnableRegeneration : 1;

  UE_API void ResetRegenerationState();

  UE_API void EvaluateRegeneration(float DeltaTime);

  UE_API float GetRegenerationSpeed() const;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UARHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UE_API UARHealthComponent();

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedDelegate, UARHealthComponent*, HealthComponent, AActor*, Instigator, float, OldHealthValue, float, NewHealthValue);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadDelegate, AActor*, OwningActor);

  UPROPERTY(BlueprintAssignable)
  FOnHealthChangedDelegate OnHealthChanged;

  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnDeadEventStarted;

  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnDeadEventFinished;

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Health")
  static UE_API UARHealthComponent* FindHealthComponent(AActor* OwningActor);

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetHealth() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetMaxHealth() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetHealthNormalized() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API bool IsDead() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API bool IsHealthMax() const;

  UE_API void HandleHealthChange(AActor* Instigator, float ChangeValue);

  UE_API void HandleOutOfHealth(AActor* OwningActor);

  UE_API void StartDead();

  UE_API void FinishDead();

  UE_API void SetAutoRegenerationEnable(const bool bEnable);

  /**TODO Temporary variable */
  bool bAutoRegenerationEnable = true;
  /** */

protected:

  /**Start UActorComponent Interface */
	UE_API virtual void BeginPlay() override;
  UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  /**End UActorComponent Interface */

private:
  void SetHealthInternal(float NewHealth);
  void SetMaxHealthInternal(float NewMaxHealth);

private:
  UPROPERTY(EditAnywhere, Category = "ARRanger|Health", meta = (AllowPrivateAccess = "true"))
  float MaxHealth;

  UPROPERTY(VisibleAnywhere, Category = "ARRanger|Health")
  float Health;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Health", meta = (AllowPrivateAccess = "true"))
  bool bAutoRegeneration;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Health", meta = (EditCondition = "bAutoRegeneration == true", EditConditionHides))
  FARHealthRegenerationEntry RegenerationEntry;
		
};

#undef UE_API