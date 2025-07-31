//*************************************************
// 斥力引力の挙動を処理するコンポーネント
//*************************************************

#include "ARObjectComponent.h"

void UARObjectComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction)
{
	Super::TickComponent(deltaTime, tickType, thisTickFunction);

	/*
	* 引力処理
	*/ 
	if (_CurrentARType == EARType::Attraction)
	{
		
	}
	/*
	* 斥力処理
	*/ 
	else if (_CurrentARType == EARType::Repulsion)
	{
		
	}
}

/*
* 新しい斥力引力状態を設定する関数
*/
void UARObjectComponent::SetARType(EARType newType)
{
	_CurrentARType = newType;
}


/*
* ARObjctComponent Lifecycle Functions
*/
UARObjectComponent::UARObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UARObjectComponent::BeginPlay()
{
	Super::BeginPlay();
	_CurrentARType = EARType::None;
}
