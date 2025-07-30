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
  PrimaryActorTick.TickGroup = ETickingGroup::TG_PrePhysics;

}

// Called when the game starts or when spawned
void AARPhysicsTickProcessorActor::BeginPlay()
{
  // SpawnActorDeferredを使用して生成するように
  ensureMsgf(OwningPhysicsEngine.IsValid(), TEXT("Create this Actor by using SpawnActorDeferred"));

	Super::BeginPlay();
	
}

// Called every frame
void AARPhysicsTickProcessorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

