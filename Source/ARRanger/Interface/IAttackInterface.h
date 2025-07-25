//*************************************************
// 攻撃をするためのインターフェース
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttackInterface.generated.h"

/*
* BPから継承可能
*/
UINTERFACE(MinimalAPI)
class UAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class IAttackInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IAttack")
	void Attack() ;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IAttack")
	bool CanAttack() const ;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IAttack")
	float GetAttackCoolTime() const;
};