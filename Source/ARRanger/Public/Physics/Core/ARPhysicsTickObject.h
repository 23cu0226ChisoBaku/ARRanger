/**
 * @file ARPhysicsTickObject.h
 * @author MAI ZHICONG
 * @brief Base class of ARPhysics tick object
 * 
 * Tickオブジェクトの基底クラス
 */

#pragma once

#include "UObject/NoExportTypes.h"

#include "Physics/Core/ARPhysicsTickTypes.h"

#include "ARPhysicsTickObject.generated.h"

#define ARPHYSIC_API ARRANGER_API

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
    ARPHYSIC_API UARPhysicsTickObject();

    /**
     * @brief TickFunctionオブジェクトをマネージャーに登録する
     */
    ARPHYSIC_API void RegisterPhysicsTickFunction();

    /**
     * @brief 物理タスクTick関数。FARPhysicsTickFunctionに呼ばれる
     * @param TickParams Tickパラメータ構造体
     */
    ARPHYSIC_API void TickPhysics(const FARPhysicsTickParameters& TickParams);
    
    /**
     * @brief 登録したTickを解読する
     * @see   RegisterPhysicsTickFunction
     */
    ARPHYSIC_API void UnregisterPhysicsTickFunction();

    /**
     * @brief Tickの更新頻度を設定する
     * @param InFrequency 
     */
    ARPHYSIC_API void SetFrequency(EPhysicsExecuteFrequency InFrequency);
    
    /**
     * @brief 前のフレームに評価された結果を返す
     * @return FARPhysicsEvaluationResult 
     */
    FARPhysicsEvaluationResult GetLastFrameResult() const { return PreviousResult; }

    /**
     * @brief 今のフレームに評価された結果を返す
     * ※評価の途中で呼ばれたら前のフレームの結果が返される
     * @return FARPhysicsEvaluationResult 
     */
    FARPhysicsEvaluationResult GetEvaluatedResult() const { return EvaluatedResult; }

    /**
     * @brief 今のフレーム評価が完了したかを確認する
     * @return 評価が完了したら true,それ以外は false
     */
    bool IsCurrentFrameEvaluateFinished() const { return bIsEvaluateFinishedCurrentFrame; }

    bool IsTickFunctionRegistered() const { return PrimaryPhysicsTick.IsTickFunctionRegistered(); }
    
    /**
     * @brief Tickのブループリント部分実装
     */
    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Physics Tick"))
    ARPHYSIC_API void TickOnBlueprint(float DeltaTime, FARPhysicsEvaluationResult& Result);
    
  private:

    /**
     * @brief Tickが呼ばれる前に実行する 
     * OnPreTickObjectを呼び出す 
     * 
     */
    void PreTickObject();

    /**
     * @brief TickFunctionが登録されたら毎フレーム呼び出される
     * OnTickが呼ばれる
     * 
     * @param TickParams Tickパラメータ構造体
     */
    void Tick(const FARPhysicsTickParameters& TickParams);

    /**
     * @brief Tickが呼ばれた後に実行する
     * OnPostTickObjectを呼び出す
     */
    void PostTickObject();
    
  protected:
    /**Start UObject interface */
    ARPHYSIC_API virtual void BeginDestroy() override;
    /**End UObject interface */
    
    ARPHYSIC_API virtual void OnPreTickObject() { }
    ARPHYSIC_API virtual void OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result) { }
    ARPHYSIC_API virtual void OnPostTickObject() { }
    
  private:
    UPROPERTY(VisibleAnywhere)
    FARPhysicsEvaluationResult PreviousResult;

    UPROPERTY(VisibleAnywhere)
    FARPhysicsEvaluationResult EvaluatedResult;

    uint8 bIsEvaluateFinishedCurrentFrame : 1 = false;
};

#undef ARPHYSIC_API