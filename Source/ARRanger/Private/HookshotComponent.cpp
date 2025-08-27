//*************************************************
// 引力フックショット(仮)の挙動を行うコンポーネント
//*************************************************

#include "HookshotComponent.h"

UHookshotComponent::UHookshotComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UHookshotComponent::BeginPlay()
{
	Super::BeginPlay();
    //m_OwnerActor = GetOwer();
}
void UHookshotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHookshotComponent::HookshotAction(AActor* targetActor)
{
	if(!m_CanHookshot)
	{
		return;
	}


}

/**
 * @brief 始点から終点に対しての方向ベクトルを計算
 * 
 * @param 始点,終点
 * 
 * @return 正規化した方向ベクトル
 */
FVector UHookshotComponent::CalculationDirection(FVector StartPos, FVector EndPos)
{
	return (StartPos - EndPos).GetSafeNormal();
}