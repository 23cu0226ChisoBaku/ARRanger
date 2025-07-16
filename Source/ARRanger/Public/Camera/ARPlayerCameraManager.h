// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/PlayerCameraManager.h"

#include "ARPlayerCameraManager.generated.h"

UCLASS()
class ARRANGER_API AARPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

  public:
    AARPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
  protected:
    virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

  private:
    float m_timeInterval = 0.0f;
};
