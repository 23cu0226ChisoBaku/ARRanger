// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ARPawnInitComponent.h"

#include "Input/ARPlayerInputBuffer.h"

// Sets default values for this component's properties
UARPawnInitComponent::UARPawnInitComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , BindInputHandles{} // TODO Move to Input Buffer
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}

UARPawnInitComponent* UARPawnInitComponent::FindPawnInitComponent(const APawn* InPawn)
{
  return nullptr;
}

void UARPawnInitComponent::OnRegister()
{

}

// Called when the game starts
void UARPawnInitComponent::BeginPlay()
{
  Super::BeginPlay();
}

void UARPawnInitComponent::InitializeASC()
{

}

void UARPawnInitComponent::InitializePlayerInput()
{

}

void UARPawnInitComponent::InitializePlayerInput(UInputComponent* InPlayerInputComponent, UARPlayerInputBuffer* PlayerInputBuffer)
{

}