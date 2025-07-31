//*************************************************
// 引力状態の挙動を実装するストラテジークラス
//*************************************************

#include "ARAttractionStrategy.h"
#include "ARObject/ARObjectBase.h"
#include "Engine/Engine.h"

void ARAttractionStrategy::Execute(AARObjectBase* owner)
{
    // ここに引力状態での処理を実装する
    UE_LOG(LogTemp, Log, TEXT("引力処理"));
}