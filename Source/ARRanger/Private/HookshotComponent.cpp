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
    m_OwnerActor = GetOwer();
}
void UHookshotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHookshotComponent::HookshotAction(AActor* targetActor)
{

}

FVector UHookshotComponent::CalculationDirection()
{

}
