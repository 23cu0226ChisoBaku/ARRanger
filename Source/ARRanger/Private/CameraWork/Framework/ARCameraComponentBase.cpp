// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraWork/Framework/ARCameraComponentBase.h"

// Sets default values for this component's properties
UARCameraComponentBase::UARCameraComponentBase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UARCameraComponentBase::OnRegister()
{
  Super::OnRegister();
}

void UARCameraComponentBase::BeginPlay()
{
  Super::BeginPlay();
  
}

void UARCameraComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
}

void UARCameraComponentBase::Activate(bool bReset)
{
  Super::Activate(bReset);
}

void UARCameraComponentBase::Deactivate()
{
  Super::Deactivate(); 
}

void UARCameraComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UARCameraComponentBase::BeginDestroy()
{
  Super::BeginDestroy();
}
