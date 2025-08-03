// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObject/WeakInterfacePtr.h"

#include "ARPhysicsTickProcessorActor.generated.h"

class FARPhysicsEngine;
class IARMagnetizableInterface;

enum class EPhysicsTickType
{
  TickAttraction,   // 引力
  TickRepulsion,    // 斥力
};

struct FARPhysicsTickFunctionParameters
{
  float DeltaTime;

  float TotalSimTime;
};

struct FARPhysicsTickFunction
{
  FARPhysicsTickFunction();
  FARPhysicsTickFunction(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsTickType InTickType);
  void Execute(const FARPhysicsTickFunctionParameters& TickParameter) const;
  bool IsEqual(IARMagnetizableInterface* Interface1, IARMagnetizableInterface* Interface2) const;
  bool IsValid() const;

private:
  TWeakInterfacePtr<IARMagnetizableInterface> Source = nullptr;
  TWeakInterfacePtr<IARMagnetizableInterface> Target = nullptr;
  EPhysicsTickType TickType;

  bool bIsAbleToExecute = false;
};

struct FARPhysicsCancellationHandle
{
  IARMagnetizableInterface* Interface1;
  IARMagnetizableInterface* Interface2;
};

UCLASS()
class AARPhysicsTickProcessorActor : public AActor
{
	GENERATED_BODY()

  public:	
    // Sets default values for this actor's properties
    ARRANGER_API AARPhysicsTickProcessorActor();

  
  protected:
    // Called when the game starts or when spawned

    /**Start AActor interface */
    ARRANGER_API virtual void BeginPlay() override;
    ARRANGER_API virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;
    /**End AActor interface */

    ARRANGER_API virtual void ProcessARPhysicsTasks(float DeltaTime, float SimTime);
  
  public:
    void OnSpawnActor(FARPhysicsEngine* PhysicsEnginePtr) { OwningPhysicsEngine = PhysicsEnginePtr; }
    bool IsBelongTo(const FARPhysicsEngine* PhysicsEngine) const { return OwningPhysicsEngine == PhysicsEngine; }
    ARRANGER_API void RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsTickType InTickType);
    ARRANGER_API void UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);

  private:
    void EnqueueRequestTask();
    void DequeueCancellationTask();

  private:
    FARPhysicsEngine* OwningPhysicsEngine;

};
