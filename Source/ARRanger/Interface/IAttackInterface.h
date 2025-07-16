//*************************************************
// �U�������邽�߂̃C���^�[�t�F�[�X
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttackInterface.generated.h"

// BP����p���\
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