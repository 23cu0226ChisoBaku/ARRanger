#include "Physics/Core/ARPhysicsTickObject.h"
#include "Physics/Core/ARPhysicsTickManagerInterface.h"

#include "Internal/ARLoggingHeader.h"

UARPhysicsTickObject::UARPhysicsTickObject()
  : PreviousResult{}
  , EvaluatedResult{}
{ }

void UARPhysicsTickObject::RegisterPhysicsTickFunction()
{
  check(!PrimaryPhysicsTick.IsTickFunctionRegistered());
  PrimaryPhysicsTick.TargetObject = this;
  PrimaryPhysicsTick.SetEnable(true);
  PrimaryPhysicsTick.RegisterPhysicsTickFunction();
}

void UARPhysicsTickObject::TickPhysics(const FARPhysicsTickParameters& TickParams)
{
  PreTickObject();
  Tick(TickParams);
  PostTickObject();
}

void UARPhysicsTickObject::UnregisterPhysicsTickFunction()
{
  PrimaryPhysicsTick.UnregisterPhysicsTickFunction();
}

void UARPhysicsTickObject::SetFrequency(EPhysicsExecuteFrequency InFrequency)
{
  PrimaryPhysicsTick.Frequency = InFrequency;
}

void UARPhysicsTickObject::PreTickObject()
{
  PreviousResult = EvaluatedResult;
  bIsEvaluateFinishedCurrentFrame = false;
  OnPreTickObject();  
}

void UARPhysicsTickObject::Tick(const FARPhysicsTickParameters& TickParams)
{
  if (!bIsEvaluateFinishedCurrentFrame)
  {
    FARPhysicsEvaluationResult result{};
    OnTick(TickParams, result);

    // For blueprint usage
    // Same as AActor::Tick
    if (GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || !GetClass()->HasAllClassFlags(CLASS_Native))
    {
      TickOnBlueprint(TickParams.DeltaTime, result);
    }

    EvaluatedResult = result;
    bIsEvaluateFinishedCurrentFrame = true;
  }
}

void UARPhysicsTickObject::PostTickObject()
{
  if (bIsEvaluateFinishedCurrentFrame)
  {
    OnPostTickObject();
  }
}

void UARPhysicsTickObject::BeginDestroy()
{
  UnregisterPhysicsTickFunction();

  Super::BeginDestroy();
}

void FARPhysicsTickFunction::ExecuteTick(const FARPhysicsTickParameters& TickParams)
{
  if (IsValid(TargetObject))
  {
    TargetObject->TickPhysics(TickParams);
  }
}
