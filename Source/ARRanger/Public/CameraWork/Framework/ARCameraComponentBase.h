// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"

#include "ARCameraComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API UARCameraComponentBase : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UARCameraComponentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
  /**Start of ActorComponent interface */
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  virtual void Activate(bool bReset=false) override;
  virtual void Deactivate() override;
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  /**End of ActorComponent interface */
	
  /**Start of UObject interface */
  virtual void BeginDestroy() override;
  /**End of UObject interface */

public:


};
