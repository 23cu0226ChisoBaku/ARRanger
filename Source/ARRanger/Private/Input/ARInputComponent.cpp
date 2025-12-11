#include "Input/ARInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARInputComponent)

UARInputComponent::UARInputComponent(const FObjectInitializer& ObjectInitializer)
  :Super(ObjectInitializer)
{ }

void UARInputComponent::RemoveBindings(TArray<uint32>& OutBoundHandles)
{
  for (const uint32 handle : OutBoundHandles)
  {
    RemoveBindingByHandle(handle);
  }

  OutBoundHandles.Reset();
}