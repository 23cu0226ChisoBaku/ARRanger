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

    void BeginPlay() override;

    /** 
     * @brief 見えない壁(鳥かご)の当たり判定を 有効 / 無効にする *
     * 
     * @param 有効 or 無効 
     */
    UFUNCTION(BlueprintCallable)
    void EnableCageCollision(bool enable);
};