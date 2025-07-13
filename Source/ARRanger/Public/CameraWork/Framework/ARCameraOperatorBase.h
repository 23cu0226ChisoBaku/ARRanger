// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "ARCameraOperatorBase.generated.h"

class UARCameraComponent;

UCLASS()
class ARRANGER_API AARCameraOperatorBase : public AActor
{
	GENERATED_BODY()
	
public:

  AARCameraOperatorBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
  /**Start of AActor interface */
  virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
  /**End of AActor interface */

  static UARCameraComponent* GetARCameraComponentStatic(const AARCameraOperatorBase* CameraOperator);

protected:

  virtual UARCameraComponent* GetARCameraComponent() const;

};
