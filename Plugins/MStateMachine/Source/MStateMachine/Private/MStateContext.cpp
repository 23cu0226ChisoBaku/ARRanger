// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateContext.h"
#include "GameplayTagContainer.h"
#include "MStateMachineComponent.h"

FMStateContext::FMStateContext()
  : m_weakOwner{nullptr}
  , m_weakController{nullptr}
  , m_stateMachineComponent{nullptr}
  , m_bIsValid{false}
{ }

FMStateContext::~FMStateContext()
{ }

void FMStateContext::InitializeContext(const FMStateContextInitializeParameters& InitializeParams)
{
  if (m_bIsValid)
  {
    return;
  }

  m_weakOwner = InitializeParams.Owner;
  m_weakController = InitializeParams.OwnerController;
  m_stateMachineComponent = InitializeParams.StateMachineComponent;
  m_bIsValid = true;
}

UWorld* FMStateContext::GetWorld() const
{
  return m_weakOwner.IsValid() ? m_weakOwner->GetWorld() : nullptr;
}

int32 FMStateContext::GetAvailableTransitionTags(TArray<FGameplayTag>& OutTags) const
{
  OutTags.Reset();

  return OutTags.Num();
}
