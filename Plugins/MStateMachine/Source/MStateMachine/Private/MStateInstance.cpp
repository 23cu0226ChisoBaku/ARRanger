// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateInstance)

UMStateInstance::UMStateInstance(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
}

void UMStateInstance::InitializeState(const FStateInitializationParameters& InitParams)
{
  OnInitializeState(InitParams);
}

void UMStateInstance::UninitializeState(const FStateUninitializationParameters& UninitParams)
{
  OnUninitializeState(UninitParams);
}

void UMStateInstance::EnterState(const FStateTransitionParameters& TransParams)
{
  OnEnterState(TransParams);
}

void UMStateInstance::TickState(const FStateTickParameters& TickParams)
{
  OnTickState(TickParams);
}

void UMStateInstance::ExitState(const FStateTransitionParameters& TransParams)
{
  OnExitState(TransParams);
}

