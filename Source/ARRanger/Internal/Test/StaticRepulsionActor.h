// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Magnetic/IARMagnetizableInterface.h"

#include "StaticRepulsionActor.generated.h"

UCLASS()
class ARRANGER_API AStaticRepulsionActor : public AActor, public IARMagnetizableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStaticRepulsionActor();

  virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override;
  AActor* GetActor() override { return this; }

private:
  UPROPERTY(EditAnywhere)
  TObjectPtr<class UBoxComponent> BoxComp;

};
