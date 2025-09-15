//*************************************************
// バトルイベントの見えない壁(鳥かご)
//*************************************************
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleEventCage.generated.h"

UCLASS()
class ABattleEventCage : public AActor
{
    GENERATED_BODY()

public:

    /** 
     * @brief 見えない壁(鳥かご)の当たり判定を 有効 / 無効にする *
     * 
     * @param 有効 or 無効 
     */
    UFUNCTION(BlueprintCallable)
    void SetCollisionEnabled(bool enable);
};