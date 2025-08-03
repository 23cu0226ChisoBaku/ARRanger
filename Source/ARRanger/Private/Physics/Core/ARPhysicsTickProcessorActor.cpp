// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/Core/ARPhysicsTickProcessorActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARPhysicsTickProcessorActor)

// Sets default values
AARPhysicsTickProcessorActor::AARPhysicsTickProcessorActor()
  : OwningPhysicsEngine{nullptr}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  // エンジンの物理演算を行う前に処理する
  PrimaryActorTick.TickGroup = TG_PrePhysics;
  bAsyncPhysicsTickEnabled = true;
}

// Called when the game starts or when spawned
void AARPhysicsTickProcessorActor::BeginPlay()
{
  // SpawnActorDeferredを使用して生成するように
  ensureMsgf(OwningPhysicsEngine != nullptr, TEXT("Create this Actor by using SpawnActorDeferred"));

	Super::BeginPlay();
	
}

void AARPhysicsTickProcessorActor::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
  // 物理演算タスクを実行する
  ProcessARPhysicsTasks(DeltaTime, SimTime);

  Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
}

void AARPhysicsTickProcessorActor::ProcessARPhysicsTasks(float DeltaTime, float SimTime)
{
  DequeueCancellationTask();
  EnqueueRequestTask();

  FARPhysicsTickFunctionParameters params;
  params.DeltaTime = DeltaTime;
  params.TotalSimTime = SimTime;
}

void AARPhysicsTickProcessorActor::RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsTickType InTickType)
{
  if (OwningPhysicsEngine == nullptr)
  {
    return;
  }

}

void AARPhysicsTickProcessorActor::UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  
}

void AARPhysicsTickProcessorActor::EnqueueRequestTask()
{

}

void AARPhysicsTickProcessorActor::DequeueCancellationTask()
{
  
}
