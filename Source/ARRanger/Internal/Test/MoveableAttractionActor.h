// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magnetic/IARMagnetizableInterface.h"
#include "MoveableAttractionActor.generated.h"

UCLASS()
class ARRANGER_API AMoveableAttractionActor : public AActor, public IARMagnetizableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveableAttractionActor();

  virtual void OnAttractionEvaluated(const FARMagneticForceResult& Result) override;
  AActor* GetActor() override { return this; }

private:
  UPROPERTY(EditAnywhere)
  TObjectPtr<class USphereComponent> SphereComp;

};
