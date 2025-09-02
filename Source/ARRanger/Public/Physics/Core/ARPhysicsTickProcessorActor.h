// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObject/WeakInterfacePtr.h"

#include "ARPhysicsTickProcessorActor.generated.h"

class FARPhysicsEngine;
class IARMagnetizableInterface;
class UARMagneticTickObject;

enum class EPhysicsRegistryType;
enum class EPhysicsExecuteFrequency;

USTRUCT()
struct FARMagneticTickObjectEntry
{
  GENERATED_BODY()

  UPROPERTY()
  TObjectPtr<UARMagneticTickObject> TickObject;

  TArray<TWeakInterfacePtr<IARMagnetizableInterface>> AffectedObjectInterfaces;

  void RegisterAffectedMagnetizedObject() const;

  ARRANGER_API friend bool operator==(const FARMagneticTickObjectEntry& Lhs, const FARMagneticTickObjectEntry& Rhs);
};

UCLASS()
class AARPhysicsTickProcessorActor : public AActor
{
	GENERATED_BODY()

  public:	
    // Sets default values for this actor's properties
    ARRANGER_API AARPhysicsTickProcessorActor();

  protected:
    // Called when the game starts or when spawned

    /**Start AActor interface */
    ARRANGER_API virtual void BeginPlay() override;
    ARRANGER_API virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;
    ARRANGER_API virtual void Tick(float DeltaTime) override;
    /**End AActor interface */

    // TODO May turn these to virtual
    ARRANGER_API void PreProcessARPhysicsTasks();
    ARRANGER_API void ProcessARPhysicsTasks(float DeltaTime, float SimTime);
    ARRANGER_API void PostProcessARPhysicsTasks();
  
  public:
    void OnSpawnActor(FARPhysicsEngine* PhysicsEnginePtr) { OwningPhysicsEngine = PhysicsEnginePtr; }
    bool IsBelongTo(const FARPhysicsEngine* PhysicsEngine) const { return OwningPhysicsEngine == PhysicsEngine; }
    ARRANGER_API void RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency);
    ARRANGER_API void UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);

  private:
    void RegisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency);
    void UnregisterMagneticTarget(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);
    void RegisterQueuedTickObject();
    void UnregisterQueuedTickObject();
    FARMagneticTickObjectEntry* GetMagneticTickObjectEntry(IARMagnetizableInterface* InTarget);
    FARMagneticTickObjectEntry* AllocateMagneticTickObject(IARMagnetizableInterface* Target, TSubclassOf<UARMagneticTickObject> MagneticTickObjectClass);

  private:
    UPROPERTY()
    TArray<FARMagneticTickObjectEntry> MagneticTickObjectEntries;

    TSet<UARMagneticTickObject*> RegisterTickObjectQueue;
    TSet<UARMagneticTickObject*> UnregisterTickObjectQueue;

    FARPhysicsEngine* OwningPhysicsEngine;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UARMagneticTickObject> AttractionTickClass;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UARMagneticTickObject> RepulsionTickClass;

#if WITH_EDITOR
    ARRANGER_API void Debug_LogTickObjectMessage();
#endif

};
