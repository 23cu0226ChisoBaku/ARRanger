#pragma once

UENUM(BlueprintType)
enum class ECameraRigType : uint8
{
  Default,    // 通常状態のカメラリグ
  Dead,       // 死亡状態のカメラリグ
  Reset,      // カメラ向きリセット専用カメラリグ
  FreeAngle,  // 自由に回転できるカメラリグ(※未実装)
};