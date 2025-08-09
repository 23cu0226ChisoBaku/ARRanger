// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#ifndef _AR_PHYSICS_GAMEPLAY_GLOBAL_
#define _AR_PHYSICS_GAMEPLAY_GLOBAL_

class AARPhysicsTickProcessorActor;

namespace ARRanger::Private
{
  /**
   * @brief ARRanger物理システム初期化インターフェイス
   */
  struct FARPhysicsCore
  {
    /**
     * @brief AR物理システムをUEワールドに初期化する
     * 
     * @param World UWorldポインター
     */
    ARRANGER_API static void InitializeARPhysicsInWorld(UWorld* World);

    /**
     * @brief 物理Tick処理アクターのクラスを指定して、AR物理システムをUEワールドに初期化する
     * 
     * @param World UWorldポインター
     * @param Subclass 物理Tick処理アクタークラス
     */
    ARRANGER_API static void InitializeARPhysicsInWorldWithActorType(UWorld* World, TSubclassOf<AARPhysicsTickProcessorActor> Subclass = nullptr);

    /**
     * @brief AR物理システムの初期化を解除する
     */
    ARRANGER_API static void DeinitializeARPhysics();

    FARPhysicsCore() = delete;
    ~FARPhysicsCore() = delete;
  };
} // namespace ARRanger::Private

#endif //_AR_PHYSICS_GAMEPLAY_GLOBAL_