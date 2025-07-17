//*************************************************
// ジャンプをするためのインターフェース
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IJumpInterface.generated.h"

/*
* BPから継承可能なインターフェース(リファレンス用)
*/
UINTERFACE(MinimalAPI)
class UJumpInterface : public UInterface
{
	GENERATED_BODY()
};

class IJumpInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	void IJump();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	bool ICanJump() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IJump")
	void IJump();

};