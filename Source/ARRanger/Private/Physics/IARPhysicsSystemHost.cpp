// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/IARPhysicsSystemHost.h"
#include "Physics/Core/ARPhysicsEngine.h"

// Add default functionality here for any IARPhysicsSystemHost functions that are not pure virtual.
void IARPhysicsSystemHost::Physics_PushTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  EnsurePhysicsEngine();

  FARPhysicsRequest request;
  request.Source = InSource;
  request.Target = InTarget;
  s_physicsEngine->RequestPhysicsProcess(request);
}

void IARPhysicsSystemHost::EnsurePhysicsEngine()
{
  if (s_physicsEngine.IsValid())
  {
    return;
  }

  s_physicsEngine = ::MakeShared<FARPhysicsEngine>();
}
