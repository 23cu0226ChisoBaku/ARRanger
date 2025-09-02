// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "Physics/Core/ARPhysicsTickTypes.h"

#include "ARPhysicsTickObject.generated.h"

/**
 * @brief AR物理評価結果構造体
 */
USTRUCT(BlueprintType)
struct FARPhysicsEvaluationResult
{
  GENERATED_BODY()
  
  /**
   * @brief 評価した力の結果
   */
  UPROPERTY(VisibleAnywhere)
  FVector ForceResult; 
};

/**
 * @brief AR物理Tickオブジェクト
 */
UCLASS(MinimalAPI, Abstract)
class UARPhysicsTickObject : public UObject
{
	GENERATED_BODY()

  protected:
    FARPhysicsTickFunction PrimaryPhysicsTick;

  private:
    /**
     * @brief 内部データ
     */
    struct FInternalData
    {
      uint8 bIsEvaluateFinishedCurrentFrame : 1 = false;
    };

  public:
    ARRANGER_API UARPhysicsTickObject();

    ARRANGER_API void RegisterPhysicsTickFunction();
    ARRANGER_API void TickPhysics(const FARPhysicsTickParameters& TickParams);    
    ARRANGER_API void UnregisterPhysicsTickFunction();

    ARRANGER_API void SetFrequency(EARPhysicsTickFrequency InFrequency);
    
    FARPhysicsEvaluationResult GetLastFrameResult() const { return PreviousResult; }
    FARPhysicsEvaluationResult GetEvaluatedResult() const { return EvaluatedResult; }
    bool IsCurrentFrameEvaluateFinished() const { return m_internalData.IsValid() && m_internalData->bIsEvaluateFinishedCurrentFrame; }
    bool IsTickFunctionRegistered() const { return PrimaryPhysicsTick.IsTickFunctionRegistered(); }
    
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Physics Tick"))
    ARRANGER_API void TickOnBlueprint(float DeltaTime, float Total, FARPhysicsEvaluationResult& Result);
    
  private:
    void BeginTickObject();
    void Tick(const FARPhysicsTickParameters& TickParams);
    void EndTickObject();
    
  protected:
    /**Start UObject interface */
    ARRANGER_API virtual void BeginDestroy() override;
    /**End UObject interface */
    
    ARRANGER_API virtual void OnBeginTickObject() { }
    ARRANGER_API virtual void OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result) { }
    ARRANGER_API virtual void OnEndTickObject() { }
    
  private:
    UPROPERTY(VisibleAnywhere)
    FARPhysicsEvaluationResult PreviousResult;

    UPROPERTY(VisibleAnywhere)
    FARPhysicsEvaluationResult EvaluatedResult;

    TUniquePtr<FInternalData> m_internalData;
};
