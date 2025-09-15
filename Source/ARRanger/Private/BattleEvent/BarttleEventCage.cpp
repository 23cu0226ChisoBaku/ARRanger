//*************************************************
// バトルイベントの見えない壁(鳥かご)
//*************************************************

#include "Public/BattleEvent/BarttleEventCage.h"
#include "Components/PrimitiveComponent.h"


/** 
 * @brief 見えない壁(鳥かご)の当たり判定を 有効 / 無効にする *
 * 
 * @param 有効 or 無効 
 */
void ABarttleEventCage::SetCollisionEnabled(bool enable)
{
    TArray<UPrimitiveComponent*> components;
    GetComponents<UPrimitiveComponent>(components);

    for (UPrimitiveComponent* comp : components)
    {
        if (comp == nullptr)
        {
            continue;  
        } 
            
        if (bEnable)
        {
            /*物理判定を有効・すべてのチャンネルをブロック*/
            comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            comp->SetCollisionResponseToAllChannels(ECR_Block);
        }
        else
        {
            /*無効化*/
            comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}