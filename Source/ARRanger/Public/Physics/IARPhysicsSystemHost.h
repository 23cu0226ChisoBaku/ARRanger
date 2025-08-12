// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "IARPhysicsSystemHost.generated.h"

/**前方宣言 */
class IARMagnetizableInterface;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARPhysicsSystemHost : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief AR物理システムサービスを提供するインターフェイス
 */
class ARRANGER_API IARPhysicsSystemHost
{
	GENERATED_BODY()

  /**
   * @brief 磁力タスク計算実行頻度
   */
  enum EMagneticTaskFrequency
  {
    Once,       // 一回
    Constantly, // 常に
  };

public:

  /**
   * @brief 物理:磁力タスクを登録する（常に計算する）
   * 
   * @param InSource 磁力対象
   * @param InTarget 磁力対象
   * 
   * 計算を常に実行する、計算を止める場合はPhysics_UnregisterMagneticTaskを呼び出す
   * @see Physics_UnregisterMagneticTask(...)
   */
  void Physics_RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);

  /**
   * @brief 物理:磁力タスクを登録する（一回だけ計算する）
   * 
   * @param InSource 磁力対象
   * @param InTarget 磁力対象
   * 
   * 計算を一回だけ実行するため、実行したら自動的に止める
   */
  void Physics_RegisterMagneticTask_Once(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);
  
  /**
   * @brief 物理:磁力タスクを止める
   * 
   * @param InSource 磁力対象
   * @param InTarget 磁力対象
   * 
   * Physics_RegisterMagneticTaskで登録されたタスクだけ止めることが
   * @see Physics_RegisterMagneticTask(...)
   */
  void Physics_UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);

private:

  /**
   * @brief 磁力タスク登録実装関数
   * 
   * @param InSource 磁力対象
   * @param InTarget 磁力対象
   * @param Frequency 更新頻度 @see EMagneticTaskFrequency
   */
  void Physics_RequestMagneticTaskImpl(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EMagneticTaskFrequency Frequency);
};
