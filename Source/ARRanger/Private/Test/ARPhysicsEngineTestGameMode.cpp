// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/ARPhysicsEngineTestGameMode.h"

#include "Physics/Gameplay/ARPhysicsGlobal.h"


void AARPhysicsEngineTestGameMode::StartPlay()
{
  Super::StartPlay();

  ARRanger::Private::FARPhysicsCore::InitializeARPhysicsInWorldWithActorType(GetWorld(), TickProcessorClass);
}

void AARPhysicsEngineTestGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  ARRanger::Private::FARPhysicsCore::DeinitializeARPhysics();
}

