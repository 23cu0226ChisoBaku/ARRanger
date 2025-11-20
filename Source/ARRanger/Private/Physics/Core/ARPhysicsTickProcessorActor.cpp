#include "Physics/Core/ARPhysicsTickProcessorActor.h"

#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"
#include "Physics/Core/ARPhysicsTickManagerInterface.h"
#include "Magnetic/IARMagnetizableInterface.h"

#include "Physics/TickObjects/Magnetic/ARMagneticAttractionTickObject.h"
#include "Physics/TickObjects/Magnetic/ARMagneticRepulsionTickObject.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARPhysicsTickProcessorActor)

using ARRanger::Physics::IARPhysicsTickManagerInterface;

void FARMagneticTickObjectEntry::RegisterAffectedMagnetizedObject() const
{
  if (TickObject != nullptr)
  {
    for (const auto& affectedObjWeakPtr : AffectedObjectInterfaces)
    {
      if (affectedObjWeakPtr.IsValid())
      {
        TickObject->RegisterAffectedMagnetizedObject(affectedObjWeakPtr.Get());
      }
    }  
  }
}

bool operator==(const FARMagneticTickObjectEntry& Lhs, const FARMagneticTickObjectEntry& Rhs)
{
  return Lhs.TickObject == Rhs.TickObject;
}

AARPhysicsTickProcessorActor::AARPhysicsTickProcessorActor()
{

	PrimaryActorTick.bCanEverTick = true;

  // エンジンの物理演算を行う前に処理する
  PrimaryActorTick.TickGroup = TG_PrePhysics;
  bAsyncPhysicsTickEnabled = true;

  AttractionTickClass = UARMagneticAttractionTickObject::StaticClass();
  RepulsionTickClass = UARMagneticRepulsionTickObject::StaticClass();
}

void AARPhysicsTickProcessorActor::PreProcessARPhysicsTasks()
{
  UnregisterQueuedTickObject();
  RegisterQueuedTickObject();

  const int32 entryNum = MagneticTickObjectEntries.Num();
  for (int32 idx = 0; idx < entryNum; ++idx)
  {
    MagneticTickObjectEntries[idx].RegisterAffectedMagnetizedObject();
  }
}

void AARPhysicsTickProcessorActor::ProcessARPhysicsTasks(float DeltaTime)
{
  IARPhysicsTickManagerInterface& tickManagerInterface = IARPhysicsTickManagerInterface::Get();
  FARPhysicsTickParameters params{};
  params.DeltaTime = DeltaTime;

  // Process Magnetic
  {
    tickManagerInterface.ExecuteARPhysicsTick(EARPhysicsTickType::TT_Magnetic, params);
  }

  // Process Gravity
  {
    tickManagerInterface.ExecuteARPhysicsTick(EARPhysicsTickType::TT_Gravity, params);
  }
}

void AARPhysicsTickProcessorActor::PostProcessARPhysicsTasks()
{
  for (const auto& entry : MagneticTickObjectEntries)
  {
    if (entry.TickObject != nullptr)
    {
      if (!entry.TickObject->IsTickFunctionRegistered())
      {
        UnregisterTickObjectQueue.Emplace(entry.TickObject);
      }
    }
  }
}

void AARPhysicsTickProcessorActor::Tick(float DeltaTime)
{
  // Trace profile
  TRACE_CPUPROFILER_EVENT_SCOPE(AARPhysicsTickProcessorActor::Tick);

  {
    PreProcessARPhysicsTasks();
    // 物理演算タスクを実行する
    ProcessARPhysicsTasks(DeltaTime);
    PostProcessARPhysicsTasks();
  }

}

void AARPhysicsTickProcessorActor::RegisterMagneticTask(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency)
{
  /**お互いに影響を及ぼすため、両方を登録する */
  RegisterMagneticTarget(InTarget, InAffectedObj, InRequestType, InFrequency);
  RegisterMagneticTarget(InAffectedObj, InTarget, InRequestType, InFrequency);
}

void AARPhysicsTickProcessorActor::UnregisterMagneticTask(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj)
{
  /**お互いに影響を及ぼすため、両方を解読する */
  UnregisterMagneticTarget(InTarget, InAffectedObj);
  UnregisterMagneticTarget(InAffectedObj, InTarget);
}

void AARPhysicsTickProcessorActor::RegisterQueuedTickObject()
{
  for (const auto& queuedTickObj : RegisterTickObjectQueue)
  {
    if (queuedTickObj != nullptr)
    {
      // TickFunction登録
      queuedTickObj->RegisterPhysicsTickFunction();
    }
  }

  RegisterTickObjectQueue.Reset();
}

void AARPhysicsTickProcessorActor::UnregisterQueuedTickObject()
{
  for (const auto& queuedTickObj : UnregisterTickObjectQueue)
  {
    if (queuedTickObj != nullptr)
    {
      // TickFunction解読
      queuedTickObj->UnregisterPhysicsTickFunction();

      FARMagneticTickObjectEntry* foundEntry = GetMagneticTickObjectEntry(queuedTickObj->GetTarget());
      if (foundEntry != nullptr)
      {
        // TickObjectを解放する
        foundEntry->TickObject->ConditionalBeginDestroy();
        MagneticTickObjectEntries.RemoveSingle(*foundEntry);
      }
    }
  }

  UnregisterTickObjectQueue.Reset();
}

void AARPhysicsTickProcessorActor::RegisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency)
{
  if (InTarget == nullptr)
  {
    AR_LOG(LogARPhysics, Warning, TEXT("Magnetizable target is invalid. Register denied"))
    return;
  }

  FARMagneticTickObjectEntry* foundEntry = GetMagneticTickObjectEntry(InTarget);
  if (foundEntry == nullptr)
  {
    TSubclassOf<UARMagneticTickObject> allocateClass = nullptr;
    using enum EPhysicsRegistryType;
    switch (InRequestType)
    {
      case RequestAttraction:
      {
        allocateClass = AttractionTickClass;
      }
      break;
      case RequestRepulsion:
      {
        allocateClass = RepulsionTickClass;
      }
      break;
    }

    foundEntry = AllocateMagneticTickObject(InTarget, allocateClass);
  }

  check(foundEntry != nullptr);
  foundEntry->AffectedObjectInterfaces.AddUnique(InAffectedObj);
  foundEntry->TickObject->SetFrequency(InFrequency); 
}

void AARPhysicsTickProcessorActor::UnregisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj)
{
  check(InTarget != nullptr);
  check(InAffectedObj != nullptr);

  FARMagneticTickObjectEntry* foundEntry = GetMagneticTickObjectEntry(InTarget);
  if (foundEntry != nullptr)
  {
    foundEntry->AffectedObjectInterfaces.RemoveSingle(InAffectedObj);

    // ターゲットが影響を与えるオブジェクトがなくなったらターゲットのTickObjectを解読キューに入れる
    if (foundEntry->AffectedObjectInterfaces.Num() < 1)
    {
      UnregisterTickObjectQueue.Emplace(foundEntry->TickObject);
    }
  }
}

FARMagneticTickObjectEntry* AARPhysicsTickProcessorActor::GetMagneticTickObjectEntry(IARMagnetizableInterface* InTarget)
{
  check(InTarget != nullptr);
  auto searchFunctor = [InTarget](const FARMagneticTickObjectEntry& Entry)
  {
    return Entry.TickObject->IsSameTarget(InTarget);
  };

  return MagneticTickObjectEntries.FindByPredicate(searchFunctor);
}

FARMagneticTickObjectEntry* AARPhysicsTickProcessorActor::AllocateMagneticTickObject(IARMagnetizableInterface* Target, TSubclassOf<UARMagneticTickObject> MagneticTickObjectClass)
{
  check(MagneticTickObjectClass != nullptr);
  UARMagneticTickObject* newTickObject = NewObject<UARMagneticTickObject>(this, MagneticTickObjectClass);
  check(newTickObject != nullptr);

  newTickObject->SetTargetObject(Target);
  RegisterTickObjectQueue.Emplace(newTickObject);

  FARMagneticTickObjectEntry newEntry{};
  newEntry.TickObject = newTickObject;
  const int32 index = MagneticTickObjectEntries.Emplace(newEntry);

  return &MagneticTickObjectEntries[index];
}
