// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/Core/ARPhysicsTickProcessorActor.h"

#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"
#include "Physics/Core/ARPhysicsTickManagerInterface.h"
#include "IARMagnetizableInterface.h"

#include "Physics/TickObjects/Magnetic/ARMagneticAttractionTickObject.h"
#include "Physics/TickObjects/Magnetic/ARMagneticRepulsionTickObject.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARPhysicsTickProcessorActor)

using ARRanger::Physics::FARPhysicsTickManagerInterface;

void FARMagneticTickObjectEntry::RegisterAffectedMagnetizedObject() const
{
  if (TickObject == nullptr)
  {
    return;
  }

  for (const auto& affectedObjWeakPtr : AffectedObjectInterfaces)
  {
    if (affectedObjWeakPtr.IsValid())
    {
      TickObject->RegisterAffectedMagnetizedObject(affectedObjWeakPtr.Get());
    }
  }
}

bool operator==(const FARMagneticTickObjectEntry& Lhs, const FARMagneticTickObjectEntry& Rhs)
{
  return Lhs.TickObject == Rhs.TickObject;
}

// Sets default values
AARPhysicsTickProcessorActor::AARPhysicsTickProcessorActor()
  : OwningPhysicsEngine{nullptr}
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  // エンジンの物理演算を行う前に処理する
  PrimaryActorTick.TickGroup = TG_PrePhysics;
  bAsyncPhysicsTickEnabled = true;

  AttractionTickClass = UARMagneticAttractionTickObject::StaticClass();
  RepulsionTickClass = UARMagneticRepulsionTickObject::StaticClass();
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
  Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
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

void AARPhysicsTickProcessorActor::ProcessARPhysicsTasks(float DeltaTime, float SimTime)
{
  FARPhysicsTickManagerInterface& tickManager = FARPhysicsTickManagerInterface::Get();
  FARPhysicsTickParameters params;
  params.DeltaTime = DeltaTime;
  params.TotalSimTime = SimTime;

  // Process Magnetic
  {
    tickManager.ExecuteARPhysicsTick(EARPhysicsTickType::TT_Magnetic, params);
  }

  // Process Gravity
  {
    tickManager.ExecuteARPhysicsTick(EARPhysicsTickType::TT_Gravity, params);
  }
}

void AARPhysicsTickProcessorActor::PostProcessARPhysicsTasks()
{
  #if WITH_EDITOR
    Debug_LogTickObjectMessage();
  #endif

  // FIXME 一時的なコード、後ほど別のところで一回しか実行しないTickObjectの管理をする
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

  PreProcessARPhysicsTasks();

  // 物理演算タスクを実行する
  ProcessARPhysicsTasks(DeltaTime, 0.0f);

  PostProcessARPhysicsTasks();

}


void AARPhysicsTickProcessorActor::RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency)
{
  RegisterMagneticTarget(InSource, InTarget, InRequestType, InFrequency);
  RegisterMagneticTarget(InTarget, InSource, InRequestType, InFrequency);
}

void AARPhysicsTickProcessorActor::UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  UnregisterMagneticTarget(InSource, InTarget);
  UnregisterMagneticTarget(InTarget, InSource);
}

void AARPhysicsTickProcessorActor::RegisterQueuedTickObject()
{
  for (const auto& queuedTickObj : RegisterTickObjectQueue)
  {
    if (queuedTickObj != nullptr)
    {
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
      queuedTickObj->UnregisterPhysicsTickFunction();
      auto searchFunctor = [&queuedTickObj](const FARMagneticTickObjectEntry& Entry)
      {
        return Entry.TickObject == queuedTickObj;
      };

      FARMagneticTickObjectEntry* foundEntry = MagneticTickObjectEntries.FindByPredicate(searchFunctor);
      if (foundEntry != nullptr)
      {
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
    // TODO
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

  switch (InFrequency)
  {
    case EPhysicsExecuteFrequency::Once:
    {
      foundEntry->TickObject->SetFrequency(EARPhysicsTickFrequency::TF_Once);
    }
    break;

    case EPhysicsExecuteFrequency::Constantly:
    {
      foundEntry->TickObject->SetFrequency(EARPhysicsTickFrequency::TF_Default);
    }
    break;

    // Not allowed Frequency that is not implemented
    default:
    {
      check(false);
    } 
    break;
  }

  
}

void AARPhysicsTickProcessorActor::UnregisterMagneticTarget(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  check(InSource != nullptr);
  check(InTarget != nullptr);

  FARMagneticTickObjectEntry* foundEntry = GetMagneticTickObjectEntry(InSource);
  if (foundEntry != nullptr)
  {
    foundEntry->AffectedObjectInterfaces.RemoveSingle(InTarget);
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
  newTickObject->SetPhysicsEngineProxy(OwningPhysicsEngine->GetProxy());

  RegisterTickObjectQueue.Emplace(newTickObject);

  FARMagneticTickObjectEntry newEntry;
  newEntry.TickObject = newTickObject;
  int32 index = MagneticTickObjectEntries.Emplace(newEntry);

  // TODO Mayby dangerous
  return &MagneticTickObjectEntries[index];
}


#if WITH_EDITOR

void AARPhysicsTickProcessorActor::Debug_LogTickObjectMessage()
{
  // if (GEngine && MagneticTickObjectEntries.Num() > 0)
  // {
  //   FString debugMsg = FString::Printf(TEXT("Tick object count: %d"), MagneticTickObjectEntries.Num());
  //   GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *debugMsg);
  // }
}

#endif