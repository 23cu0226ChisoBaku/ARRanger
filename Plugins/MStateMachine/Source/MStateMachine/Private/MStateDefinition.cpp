// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateDefinition.h"

#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateDefinition)

FString FMStateTransitionInfo::ToString() const
{
  return StateTag.IsValid()? StateTag.ToString() : FGameplayTag::EmptyTag.ToString();
}

UMStateDefinition::UMStateDefinition()
{
  InstanceType = UMStateInstance::StaticClass();
}