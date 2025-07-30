#pragma once

#include "IARTypeInterface.generated.h"

/*
* 斥力引力状態
*/
UENUM()
enum class EARType : uint8
{
	None       = 0,  // 何もない状態(デフォルト値)
	Attraction = 1,  // 引力状態
	Repulsion  = 2,  // 斥力状態
};

/*
* 内容：新しい状態に変化させる関数
* 引数：変化先に状態
*/
void SetNewARType(EARType newARType);
