// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/Core/ARPhysicsTickProcessorActor.h"

#include "Physics/Core/ARPhysicsEngine.h"
#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"
#include "Physics/Core/ARPhysicsTickManagerInterface.h"

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
    TickObject->RegisterAffectedMagnetizedObject(affectedObjWeakPtr.Get());
  }
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

  AttractionTickClass = UARMagneticTickObject::StaticClass();
  RepulsionTickClass = UARMagneticTickObject::StaticClass();
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
  PreProcessARPhysicsTasks();

  // 物理演算タスクを実行する
  ProcessARPhysicsTasks(DeltaTime, SimTime);

  Super::AsyncPhysicsTickActor(DeltaTime, SimTime);
}

void AARPhysicsTickProcessorActor::PreProcessARPhysicsTasks()
{
  for (const auto& entry : MagneticTickObjectEntries)
  {
    entry.RegisterAffectedMagnetizedObject();
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

void AARPhysicsTickProcessorActor::RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsRequestType InRequestType)
{
  if (OwningPhysicsEngine == nullptr)
  {
    return;
  }

  RegisterMagneticTarget(InSource, InTarget, InRequestType);
  RegisterMagneticTarget(InTarget, InSource, InRequestType);

}

void AARPhysicsTickProcessorActor::UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget)
{
  
}

void AARPhysicsTickProcessorActor::RegisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRequestType InRequestType)
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
    using enum EPhysicsRequestType;
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
  foundEntry->AffectedObjectInterfaces.Emplace(InAffectedObj);
  
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
  newTickObject->RegisterPhysicsTickFunction();

  FARMagneticTickObjectEntry newEntry;
  newEntry.TickObject = newTickObject;
  int32 index = MagneticTickObjectEntries.Emplace(newEntry);

  // TODO Mayby dangerous
  return &MagneticTickObjectEntries[index];
}
