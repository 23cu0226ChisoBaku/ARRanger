// ステートの本体
// K2_：C++から呼び出されるBlueprint実装用の関数に使われる UEの命名規則 

#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateInstance)

UMStateInstance::UMStateInstance(const FObjectInitializer& objectInitializer)
  : Super(objectInitializer)
{
}

void UMStateInstance::BeginDestroy()
{
  Super::BeginDestroy();
}

void UMStateInstance::EntryState()
{
  K2_EntryState();
}

void UMStateInstance::TickState(float inDeltaTime)
{
  K2_TickState(inDeltaTime);
}

void UMStateInstance::ExitState()
{
  K2_ExitState();
}
