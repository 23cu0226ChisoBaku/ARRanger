//*************************************************
// ヒットストップの処理を行うヘルパークラス
//*************************************************

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "HitStopHelper.generated.h"

/*前方宣言*/
class UWorld;

UCLASS(Blueprintable)
class UHitStopHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 指定した期間、指定した速度でモーションさせる
	 * 
	 * @param 呼び出すオブジェクト、止める期間、モーション速度
	 */
	UFUNCTION(BlueprintCallable, Category = "HitStop")
	static FTimerHandle StartHitStop(UObject* object, float duration, float motionSpeed = 0.0f);
};
