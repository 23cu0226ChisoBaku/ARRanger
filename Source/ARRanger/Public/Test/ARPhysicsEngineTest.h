// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Physics/IARPhysicsSystemHost.h"
#include "IARMagnetizableInterface.h"

#include "ARPhysicsEngineTest.generated.h"

UCLASS()
class AARPhysicsEngineTest :  public AActor, 
                              public IARPhysicsSystemHost,
                              public IARMagnetizableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARRANGER_API AARPhysicsEngineTest();

protected:
	// Called when the game starts or when spawned
	ARRANGER_API virtual void BeginPlay() override;

public:	
	// Called every frame
	ARRANGER_API virtual void Tick(float DeltaTime) override;

  UFUNCTION(BlueprintCallable, Category = "Physics|Test")
  ARRANGER_API void TestRequestMagneticTask(AActor* Target);

  /**Start IARMagnetizableInterface interface*/
	ARRANGER_API virtual void OnAttractionEvaluated(const FARMagneticForceResult& Result) override;
	ARRANGER_API virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override;
	virtual AActor* GetActor() override { return this; }
  /**End IARMagnetizableInterface interface*/

};
