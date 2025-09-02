//*************************************************
// 引力必殺技の制御コンポーネント
//*************************************************

#include "PlayerComponents/AttractSpecialAttackComponent.h"

UAttractSpecialAttackComponent::UAttractSpecialAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UAttractSpecialAttackComponent::BeginPlay()
{
	Super::BeginPlay();	
}
void UAttractSpecialAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/**
 * @brief 対象のオブジェクトを引き寄せる物体を生成する
 */
void UAttractSpecialAttackComponent::GeneratAttractActor()
{
	;
}
