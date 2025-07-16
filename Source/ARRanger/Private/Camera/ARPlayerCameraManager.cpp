// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ARPlayerCameraManager.h"


AARPlayerCameraManager::AARPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{

}

void AARPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
  Super::UpdateViewTarget(OutVT, DeltaTime);

  static FVector offset = FVector::ZeroVector;
  static const FVector moveDir = FVector{0.1, 0 ,0};

  m_timeInterval += DeltaTime;

  offset = moveDir * FMath::Cos(m_timeInterval * 2 * PI);

  // ただ近接平面を直接的いじっても、平行移動のように見える
  // OutVT.POV.OffCenterProjectionOffset += FVector2D{offset.X, offset.Y};
  // OutVT.POV.FOV = 90.0f - FMath::Cos(m_timeInterval * 2 * PI) * 20.0f;
}
 

