// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ARPlayerCameraManager.h"


AARPlayerCameraManager::AARPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{

}

void AARPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
  Super::UpdateViewTarget(OutVT, DeltaTime);

  static const FVector2D moveDir = FVector2D{.3, .3};

  //ただ近接平面を直接的いじっても、平行移動のように見える
  OutVT.POV.OffCenterProjectionOffset = moveDir;
  // OutVT.POV.FOV = 200.f;
}
 

