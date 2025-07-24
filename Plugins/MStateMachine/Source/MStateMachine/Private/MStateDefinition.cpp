// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateDefinition.h"

#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateDefinition)

FString FMStateTagInfo::ToString() const
{
  return StateTag.ToString();
}

UMStateDefinition::UMStateDefinition()
{
  InstanceType = UMStateInstance::StaticClass();
}