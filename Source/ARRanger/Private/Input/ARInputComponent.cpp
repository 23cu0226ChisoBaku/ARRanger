// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/ARInputComponent.h"

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