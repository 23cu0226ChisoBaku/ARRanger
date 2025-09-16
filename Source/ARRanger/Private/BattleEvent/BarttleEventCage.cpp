//*************************************************
// バトルイベントの見えない壁(鳥かご)
//*************************************************

#include "Public/BattleEvent/BattleEventCage.h"
#include "Components/PrimitiveComponent.h"

/** 
 * @brief 見えない壁(鳥かご)の当たり判定を 有効 / 無効にする *
 * 
 * @param 有効 or 無効 
 */
void ABattleEventCage::EnableCageCollision(bool enable)
{
    TArray<UPrimitiveComponent*> components;
    GetComponents<UPrimitiveComponent>(components);

    for (UPrimitiveComponent* comp : components)
    {
        if (comp == nullptr)
        {
            continue;  
        } 
            
        if (enable)
        {
            /*物理判定を有効・すべてのチャンネルをブロック*/
            comp->SetCollisionProfileName(TEXT("BattleEventCage"));

            UE_LOG(LogTemp, Warning, TEXT("ABattleEventCage::EnableCageCollision() 有効"));
        }
        else
        {
            /*無効化*/
            comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            UE_LOG(LogTemp, Warning, TEXT("ABattleEventCage::EnableCageCollision() 無効化"));
        }
    }
}