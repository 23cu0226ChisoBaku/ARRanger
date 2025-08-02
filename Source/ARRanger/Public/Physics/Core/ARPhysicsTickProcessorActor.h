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

struct FARPhysicsTickFunctioncParameters
{
  TWeakPtr<FARPhysicsEngine> PhysicsEngine;

  float DeltaTime;
};

struct FARPhysicsTickFunction
{
  FARPhysicsTickFunction(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsTickType InTickType);
  void Execute(const FARPhysicsTickFunctioncParameters& TickParameter);

private:
  TWeakInterfacePtr<IARMagnetizableInterface> Source = nullptr;
  TWeakInterfacePtr<IARMagnetizableInterface> Target = nullptr;
  EPhysicsTickType TickType;
};

UCLASS()
class ARRANGER_API AARPhysicsTickProcessorActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AARPhysicsTickProcessorActor();

  void OnSpawnActor(FARPhysicsEngine* PhysicsEnginePtr) { OwningPhysicsEngine = PhysicsEnginePtr;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
  virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

public:
  bool IsBelongTo(const FARPhysicsEngine* PhysicsEngine) const { return false;}

private:
  FARPhysicsEngine* OwningPhysicsEngine;

  TArray<TUniquePtr<FARPhysicsTickFunction>> TickFunctions;
  
public:
  TDelegate<void()> OnDestroyTickProcessor;

};
