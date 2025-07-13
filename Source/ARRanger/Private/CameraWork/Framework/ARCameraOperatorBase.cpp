// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraWork/Framework/ARCameraOperatorBase.h"

AARCameraOperatorBase::AARCameraOperatorBase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  
}

void AARCameraOperatorBase::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
  Super::CalcCamera(DeltaTime, OutResult);
}

UARCameraComponent* AARCameraOperatorBase::GetARCameraComponentStatic(const AARCameraOperatorBase* CameraOperator)
{
  check(CameraOperator != nullptr);
  if (CameraOperator == nullptr)
  {
    return nullptr;
  }

  return CameraOperator->GetARCameraComponent();
}

UARCameraComponent* AARCameraOperatorBase::GetARCameraComponent() const
{
  return nullptr;
}