// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ARPawnInitData.h"

UARPawnInitData::UARPawnInitData(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  Abilities.Reset();
  AttributeSets.Reset();
}