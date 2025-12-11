#pragma once

#ifndef _AR_PHYSICS_TYPES_
#define _AR_PHYSICS_TYPES_

/**
 * @brief 物理タスク実行頻度
 */
enum class EPhysicsExecuteFrequency : uint8
{
  Once,         // 一回だけ実行
  Constantly,   // 常に実行
};

#endif // _AR_PHYSICS_TYPES_