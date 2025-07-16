//*************************************************
// 攻撃をするためのインターフェース
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttackInterface.generated.h"

// BPから継承可能
UINTERFACE(Blueprintable)
class UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class IAttackInterface
{
	GENERATED_BODY()

public:
	virtual void Attack() = 0;
	virtual bool CanAttack() const = 0;
	virtual float GetAttackCoolTime() const = 0;
};