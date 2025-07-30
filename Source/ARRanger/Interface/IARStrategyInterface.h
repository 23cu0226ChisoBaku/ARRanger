//*************************************************
// 引力・斥力などの挙動を表すストラテジーインターフェース
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "IARTypeInterface.h"

// 前方宣言
class AARObjectBase;

/**
 * 引力・斥力などの挙動を表すストラテジーインターフェース
 * AARObjectBaseはこのインターフェースを通じて挙動を切り替え・実行する
 */
class IARStrategyInterface
{
public:
    virtual ~IARStrategyInterface() = default;

    // 実行する挙動の関数
    virtual void Execute(AARObjectBase* owner) = 0;
    // 現在のタイプを返す
    virtual EARType GetStrategyType() const = 0;
};
