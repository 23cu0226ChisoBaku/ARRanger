// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAbilities/ARGameplayAbilityBase.h"

UARGameplayAbilityBase::UARGameplayAbilityBase()
  : bCanCancel{false}
{ }

void UARGameplayAbilityBase::SetAbilityCancelable()
{
  bCanCancel = true;
}

void UARGameplayAbilityBase::SetAbilityBlock()
{
  bCanCancel = false;
}