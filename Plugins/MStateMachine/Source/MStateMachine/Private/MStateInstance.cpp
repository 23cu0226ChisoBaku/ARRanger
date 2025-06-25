// Fill out your copyright notice in the Description page of Project Settings.


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
