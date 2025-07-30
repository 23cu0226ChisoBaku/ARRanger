#include "Physics/Core/ARPhysicsEngineProxy.h"

FARPhysicsEngineProxy::FARPhysicsEngineProxy()
  : m_privatePhysicsEngine{nullptr}
{

}

FARPhysicsEngineProxy::~FARPhysicsEngineProxy() = default;

void FARPhysicsEngineProxy::Initialize(const TSharedPtr<FARPhysicsEngine>& OwningPhysicsEngine)
{
  OnInitialize(OwningPhysicsEngine);
}

void FARPhysicsEngineProxy::SimulateAttraction(const FARPhysicsSimulationParam& SimulationParameter)
{
  OnSimulateAttraction(SimulationParameter);
}

void FARPhysicsEngineProxy::SimulateRepulsion(const FARPhysicsSimulationParam& SimulationParameter)
{
  OnSimulateRepulsion(SimulationParameter);
}

void FARPhysicsEngineProxy::OnInitialize(const TSharedPtr<FARPhysicsEngine>& OwningPhysicsEngine)
{
  m_privatePhysicsEngine = OwningPhysicsEngine;
}

void FARPhysicsEngineProxy::OnSimulateAttraction(const FARPhysicsSimulationParam& SimulationParameter)
{
  // FIXME Implement immediately
} 

void FARPhysicsEngineProxy::OnSimulateRepulsion(const FARPhysicsSimulationParam& SimulationParameter)
{
  // FIXME Implement immediately
}