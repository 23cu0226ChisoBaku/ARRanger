/**
 * @file ARPhysicsEngine.h
 * @author MAI ZHICONG
 * @brief Physics engine for Project:ARRanger
 */

#pragma once

#ifndef _AR_PHYSICS_ENGINE_
#define _AR_PHYSICS_ENGINE_

#include "UObject/WeakInterfacePtr.h"

#include "Internal/CountLimiter.h"
#include "Physics/Core/ARPhysicsTypes.h"

/**前方宣言 */
class IARMagnetizableInterface;
class IPhysicsTaskRegistrar;

/**
 * @brief 物理タスク登録タイプ
 */
enum class EPhysicsRegistryType : uint8
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
 * @brief 物理タスク登録パラメータ
 */
struct FARPhysicsRegistry
{
  /**磁力対象1 */
  IARMagnetizableInterface* Source = nullptr;

  /**磁力対象2 */
  IARMagnetizableInterface* Target = nullptr;

  /**登録タイプ */
  EPhysicsRegistryType Type = EPhysicsRegistryType::None;

  /**実行頻度 */
  EPhysicsExecuteFrequency Frequency = EPhysicsExecuteFrequency::Once;

  /**
   * @brief 物理タスク登録請求が磁力タイプか
   * 
   * @return 引力斥力だとtrue,それ以外はfalse
   */
  __forceinline bool IsMagneticForceType() const
  {
    using enum EPhysicsRegistryType;
    return (Type == RequestAttraction) || (Type == RequestRepulsion);
  }
};

/**
 * @brief 物理タスク登録解除パラメータ
 */
struct FARPhysicsUnregistry
{
  /**磁力対象1 */
  IARMagnetizableInterface* Source = nullptr;

  /**磁力対象2 */
  IARMagnetizableInterface* Target = nullptr;

  /**登録解除タイプ */
  EPhysicsUnregistryType Type = EPhysicsUnregistryType::None;
};

/**
 * @brief AR物理エンジン初期化パラメータ
 */
struct FARPhysicsEngineInitializationParameters
{
  IPhysicsTaskRegistrar* TaskRegistrar = nullptr;
};

#define ARPHYSICS_API ARRANGER_API

/**
 * @brief AR物理エンジンクラス
 * インスタンス最大数 : 1
 */
class FARPhysicsEngine : private ARRanger::Private::FCountLimiter<FARPhysicsEngine, 1>
{
  friend class FARPhysicsEngineProxy;
  using PhysicsEngineProxyPtr = FARPhysicsEngineProxy*;

  /**インスタンス数制限クラス宣言 */
  DECLARE_COUNT_LIMITER_PROPERTY(FARPhysicsEngine, 1)

  public:
    ARPHYSICS_API FARPhysicsEngine();
    ARPHYSICS_API virtual ~FARPhysicsEngine();

    /**
     * @brief 物理エンジンを初期化する
     * 
     * @param Parameters 初期化する専用パラメータ
     * @see FARPhysicsEngineInitializationParameters
     */
    ARPHYSICS_API void InitializePhysicsEngine(const FARPhysicsEngineInitializationParameters& Parameters);

    /**
     * @brief 物理エンジンを解放する
     */
    ARPHYSICS_API void DeinitializePhysicsEngine();

    /**
     * @brief 物理タスクを登録する
     * 
     * @param Registry 登録パラメータ
     * @see FARPhysicsRegistry
     */
    ARPHYSICS_API void RegisterPhysicsTask(const FARPhysicsRegistry& Registry);

    /**
     * @brief 物理タスクの登録状態を解除する
     * 
     * @param Unregistry 登録解除パラメータ
     * @see FARPhysicsUnregistry
     */
    ARPHYSICS_API void UnregisterPhysicsProcess(const FARPhysicsUnregistry& Unregistry);

  private:

    /**弱参照 */ /**物理タスクレジストラ */
    TWeakInterfacePtr<IPhysicsTaskRegistrar> m_taskRegistrar;
};

#undef ARPHYSICS_API

#endif // _AR_PHYSICS_ENGINE_
