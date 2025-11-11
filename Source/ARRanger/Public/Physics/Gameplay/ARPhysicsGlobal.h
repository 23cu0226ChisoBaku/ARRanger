// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _AR_PHYSICS_GAMEPLAY_GLOBAL_
#define _AR_PHYSICS_GAMEPLAY_GLOBAL_

class IPhysicsTaskRegistrar;

namespace ARRanger::Private
{
  /**
   * @brief ARRanger物理システム初期化インターフェイス
   */
  struct FARPhysicsCore
  {
    ARRANGER_API static void InitializeARPhysics(IPhysicsTaskRegistrar* InTaskRegistrar);

    /**
     * @brief AR物理システムの初期化を解除する
     */
    ARRANGER_API static void DeinitializeARPhysics();

    FARPhysicsCore() = delete;
    ~FARPhysicsCore() = delete;
  };
} // namespace ARRanger::Private

#endif //_AR_PHYSICS_GAMEPLAY_GLOBAL_