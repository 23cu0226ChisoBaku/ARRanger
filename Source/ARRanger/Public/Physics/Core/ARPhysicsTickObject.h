/**
 * @file ARPhysicsTickObject.h
 * @author MAI ZHICONG
 * @brief Base class of ARPhysics tick object
 */

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
   * @brief 評価した力(ベクトル3D)の結果
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
    /**Tick function to call TickPhysics */
    FARPhysicsTickFunction PrimaryPhysicsTick;

  public:
    ARRANGER_API UARPhysicsTickObject();

    /**
     * @brief Register tick function to TickManager
     */
    ARRANGER_API void RegisterPhysicsTickFunction();

    /**
     * @brief Tick function called by FARPhysicsTickFunction
     * @param TickParams 
     */
    ARRANGER_API void TickPhysics(const FARPhysicsTickParameters& TickParams);
    
    /**
     * @brief Unregister tick function in TickManager if RegisterPhysicsTickFunction() called
     * @see   RegisterPhysicsTickFunction
     */
    ARRANGER_API void UnregisterPhysicsTickFunction();

    /**
     * @brief Set the tick frequency of tick function
     * @param InFrequency 
     */
    ARRANGER_API void SetFrequency(EARPhysicsTickFrequency InFrequency);
    
    /**
     * @brief Return result evaluated in last frame
     * @return FARPhysicsEvaluationResult 
     */
    FARPhysicsEvaluationResult GetLastFrameResult() const { return PreviousResult; }

    /**
     * @brief Return result evaluated in current frame.
     * May have same value of PreviousResult
     * @return FARPhysicsEvaluationResult 
     */
    FARPhysicsEvaluationResult GetEvaluatedResult() const { return EvaluatedResult; }

    /**
     * @brief Check if evaluation is finished in current frame
     * @return bIsEvaluateFinishedCurrentFrame
     */
    bool IsCurrentFrameEvaluateFinished() const { return bIsEvaluateFinishedCurrentFrame; }

    /**
     * @brief Check if tick function is registered
     * 
     * @return true if tick function is registered, otherwise false
     */
    bool IsTickFunctionRegistered() const { return PrimaryPhysicsTick.IsTickFunctionRegistered(); }
    
    /**
     * @brief Tick event for blueprint
     */
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Physics Tick"))
    ARRANGER_API void TickOnBlueprint(float DeltaTime, FARPhysicsEvaluationResult& Result);
    
  private:

    /**
     * @brief Process before Tick call 
     * Will call virtual function OnBeginTickObject 
     */
    void BeginTickObject();

    /**
     * @brief Called every frame if ARPhysicsTask is registered
     * Will call virtual function OnTick
     * 
     * @param TickParams 
     */
    void Tick(const FARPhysicsTickParameters& TickParams);

    /**
     * @brief Called after Tick
     * Will call virtual function OnEndTickObject
     */
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

    uint8 bIsEvaluateFinishedCurrentFrame : 1 = false;
};
