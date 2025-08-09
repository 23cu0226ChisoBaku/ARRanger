// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _AR_PHYSICS_ENGINE_
#define _AR_PHYSICS_ENGINE_

#include "Internal/CountLimiter.h"

/**前方宣言 */
class IARMagnetizableInterface;
class FARPhysicsEngineProxy;
class AARPhysicsTickProcessorActor;
class UWorld;
class AARPhysicsTickProcessorActor;

/**
 * @brief 物理タスク登録タイプ
 */
enum class EPhysicsRegistryType
{
  None,               // タイプがない
  RequestAttraction,  // 引力タイプ
  RequestRepulsion,   // 斥力タイプ
};

/**
 * @brief 登録解除物理タスクタイプ
 */
enum class EPhysicsUnregistryType
{
  None,               // タイプがない
  UnregisterMagnetic, // 磁力タイプ
};

/**
 * @brief 物理タスク実行頻度
 */
enum class EPhysicsExecuteFrequency
{
  Never,        // 実行しない
  Once,         // 一回だけ実行
  Constantly,   // 常に実行
};

/**
 * @brief 物理タスク登録パラメータ
 */
struct FARPhysicsRegistry
{
  /**磁力対象 */
  IARMagnetizableInterface* Source = nullptr;

  /**磁力対象 */
  IARMagnetizableInterface* Target = nullptr;

  /**登録タイプ */
  EPhysicsRegistryType Type = EPhysicsRegistryType::None;

  /**実行頻度 */
  EPhysicsExecuteFrequency Frequency = EPhysicsExecuteFrequency::Never;

  /**
   * @brief 物理タスク登録請求が磁力タイプか
   * 
   * @return 引力斥力だとtrue,それ以外はfalse
   */
  __forceinline bool IsMagneticForceType() const
  {
    using enum EPhysicsRegistryType;
    return Type == RequestAttraction || Type == RequestRepulsion;
  }
};

/**
 * @brief 物理タスク登録解除パラメータ
 */
struct FARPhysicsUnregistry
{
  /**磁力対象 */
  IARMagnetizableInterface* Source = nullptr;

  /**磁力対象 */
  IARMagnetizableInterface* Target = nullptr;

  /**登録解除タイプ */
  EPhysicsUnregistryType Type = EPhysicsUnregistryType::None;
};

/**
 * @brief AR物理エンジン初期化パラメータ
 */
struct FARPhysicsEngineInitializationParameters
{
  /**初期化するUWorld */
  TObjectPtr<UWorld> World;

  /**物理タスクTick実行Actorのクラス */
  TSubclassOf<AARPhysicsTickProcessorActor> SubclassOfPTPActor;
};

/**
 * @brief AR物理エンジンクラス
 */
class FARPhysicsEngine : private ARRanger::Private::FCountLimiter<FARPhysicsEngine, 1>
{
  friend class FARPhysicsEngineProxy;
  using PhysicsEngineProxyPtr = FARPhysicsEngineProxy*;

  /**インスタンス数制限クラス宣言 */
  DECLARE_COUNT_LIMITER_PROPERTY(FARPhysicsEngine, 1)

  public:
    ARRANGER_API FARPhysicsEngine();
    ARRANGER_API virtual ~FARPhysicsEngine();

    /**
     * @brief 物理エンジンを初期化する
     * 
     * @param Parameters 初期化する専用パラメータ
     * @see FARPhysicsEngineInitializationParameters
     */
    ARRANGER_API void InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters);

    /**
     * @brief 物理エンジンを解放する
     */
    ARRANGER_API void DeinitializePhysicsEngine();

    /**
     * @brief 物理タスクを登録する
     * 
     * @param Registry 登録パラメータ
     * @see FARPhysicsRegistry
     */
    ARRANGER_API void RegisterPhysicsTask(const FARPhysicsRegistry& Registry);

    /**
     * @brief 物理タスクの登録状態を解除する
     * 
     * @param Unregistry 登録解除パラメータ
     * @see FARPhysicsUnregistry
     */
    ARRANGER_API void UnregisterPhysicsProcess(const FARPhysicsUnregistry& Unregistry);

    /**
     * @brief 物理エンジンプロキシ（代理）を取得
     * 
     * @return 初期化できたらプロキシを返す、それ以外はnullptr
     */
    PhysicsEngineProxyPtr GetProxy() const { return m_proxy.Get(); }

    /**
     * // FIXME:Implement immediately
     */
    PhysicsEngineProxyPtr BuildProxy() const { return m_proxy.Get(); }

  protected:
    ARRANGER_API virtual TSharedPtr<FARPhysicsEngineProxy> MakePhysicsEngineProxy() const;

  protected:
    AARPhysicsTickProcessorActor* GetTickProcessorActorPrivate() { return m_tickProcessorActor.Get(); }

  private:

    /**
     * @brief 物理タスクTickアクターを初期化
     * 
     * @param World UWorldポインター
     * @param Subclass 物理タスクTickアクターUClass
     */
    void InitializePhysicsTickProcessorActor(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass);

  private:

    /**プロキシ */
    // TODO ここで保存しないかも
    TSharedPtr<FARPhysicsEngineProxy> m_proxy;

    /**物理タスクTickアクター弱参照 */
    TWeakObjectPtr<AARPhysicsTickProcessorActor> m_tickProcessorActor;
};


#endif // _AR_PHYSICS_ENGINE_
