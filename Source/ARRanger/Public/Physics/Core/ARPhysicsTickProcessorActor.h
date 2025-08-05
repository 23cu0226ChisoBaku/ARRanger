// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UObject/WeakInterfacePtr.h"

#include "ARPhysicsTickProcessorActor.generated.h"

class FARPhysicsEngine;
class IARMagnetizableInterface;
class UARMagneticTickObject;

enum class EPhysicsRequestType;

USTRUCT()
struct FARMagneticTickObjectEntry
{
  GENERATED_BODY()

  UPROPERTY()
  TObjectPtr<UARMagneticTickObject> TickObject;

  TArray<TWeakInterfacePtr<IARMagnetizableInterface>> AffectedObjectInterfaces;

  void RegisterAffectedMagnetizedObject() const;
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
    /**End AActor interface */

    // TODO May turn these to virtual
    ARRANGER_API void PreProcessARPhysicsTasks();
    ARRANGER_API void ProcessARPhysicsTasks(float DeltaTime, float SimTime);
  
  public:
    void OnSpawnActor(FARPhysicsEngine* PhysicsEnginePtr) { OwningPhysicsEngine = PhysicsEnginePtr; }
    bool IsBelongTo(const FARPhysicsEngine* PhysicsEngine) const { return OwningPhysicsEngine == PhysicsEngine; }
    ARRANGER_API void RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsRequestType InRequestType);
    ARRANGER_API void UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget);

  private:
    void RegisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRequestType InRequestType);
    FARMagneticTickObjectEntry* GetMagneticTickObjectEntry(IARMagnetizableInterface* InTarget);
    FARMagneticTickObjectEntry* AllocateMagneticTickObject(IARMagnetizableInterface* Target, TSubclassOf<UARMagneticTickObject> MagneticTickObjectClass);

  private:
    TArray<FARMagneticTickObjectEntry> MagneticTickObjectEntries;

    FARPhysicsEngine* OwningPhysicsEngine;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UARMagneticTickObject> AttractionTickClass;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UARMagneticTickObject> RepulsionTickClass;



};
