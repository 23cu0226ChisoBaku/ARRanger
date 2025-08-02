// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/ARPhysicsEngineTestGameMode.h"

#include "Physics/IARPhysicsSystemHost.h"

void AARPhysicsEngineTestGameMode::StartPlay()
{
  Super::StartPlay();

  ARRanger::Private::FARPhysicsCore::InitializeARPhysicsInWorld(GetWorld());
}

void AARPhysicsEngineTestGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  ARRanger::Private::FARPhysicsCore::DeinitializeARPhysics();
}

