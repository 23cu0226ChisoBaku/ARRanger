#include "Physics/Core/ARPhysicsEngineProxy.h"
#include "IARMagnetizableInterface.h"

#include "Internal/ARLoggingHeader.h"

FARPhysicsSimulationParam::FARPhysicsSimulationParam(IARMagnetizableInterface& InTarget1, IARMagnetizableInterface& InTarget2)
  : Target1(InTarget1)
  , Target2(InTarget2)
{ }

FARPhysicsEngineProxy::FARPhysicsEngineProxy()
  : m_privatePhysicsEngine{nullptr}
{ }

FARPhysicsEngineProxy::~FARPhysicsEngineProxy() = default;

void FARPhysicsEngineProxy::Initialize(FARPhysicsEngine* OwningPhysicsEnginePtr)
{
  m_privatePhysicsEngine = OwningPhysicsEnginePtr;

  OnInitialize(OwningPhysicsEnginePtr);
}

void FARPhysicsEngineProxy::SimulateAttraction(const FARPhysicsSimulationParam& SimulationParameter)
{
  OnSimulateAttraction(SimulationParameter);
}

void FARPhysicsEngineProxy::SimulateRepulsion(const FARPhysicsSimulationParam& SimulationParameter)
{
  OnSimulateRepulsion(SimulationParameter);
}

void FARPhysicsEngineProxy::OnInitialize(FARPhysicsEngine* OwningPhysicsEnginePtr)
{
  // Empty implementation
}

void FARPhysicsEngineProxy::OnSimulateAttraction(const FARPhysicsSimulationParam& SimulationParameter)
{
  // SimulationParameter.Target1.OnAttractionEvaluated();
  // SimulationParameter.Target2.OnAttractionEvaluated();
} 

void FARPhysicsEngineProxy::OnSimulateRepulsion(const FARPhysicsSimulationParam& SimulationParameter)
{
  // SimulationParameter.Target1.OnRepulsionEvaluated();
  // SimulationParameter.Target2.OnRepulsionEvaluated();
}