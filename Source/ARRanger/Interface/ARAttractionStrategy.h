//*************************************************
// 引力状態の挙動を実装するストラテジークラス
//*************************************************

#pragma once

#include "Interface/IARTypeInterface.h"
#include "Interface/IARStrategyInterface.h"

class ARAttractionStrategy : public IARStrategyInterface
{
public:
    virtual void Execute(AARObjectBase* owner) override;

    virtual EARType GetStrategyType() const override { return EARType::Attraction; }
};