/**
 * @file ARPhysicsTickProcessorActor.h
 * @author MAI ZHICONG
 * @brief Actor to tick PhysicsTickManager every frame in game world
 */

#pragma once

#include "GameFramework/Actor.h"
#include "UObject/WeakInterfacePtr.h"
#include "Physics/Core/IPhysicsTaskRegistrar.h"

#include "ARPhysicsTickProcessorActor.generated.h"

/**前方宣言 */
class FARPhysicsEngine;
class IARMagnetizableInterface;
class UARMagneticTickObject;
enum class EPhysicsRegistryType : uint8;
enum class EPhysicsExecuteFrequency : uint8;


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
class AARPhysicsTickProcessorActor : public AActor , public IPhysicsTaskRegistrar
{
	GENERATED_BODY()

  public:	
    ARRANGER_API AARPhysicsTickProcessorActor();

  protected:
    /**Start AActor interface */
    ARRANGER_API virtual void Tick(float DeltaTime) override;
    /**End AActor interface */

    ARRANGER_API void PreProcessARPhysicsTasks();
    ARRANGER_API void ProcessARPhysicsTasks(float DeltaTime);
    ARRANGER_API void PostProcessARPhysicsTasks();
  
  public:
    /**Start IPhysicsTaskRegistrar Interface */
    ARRANGER_API void RegisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency) override;
    ARRANGER_API void UnregisterMagneticTask(IARMagnetizableInterface* InSource, IARMagnetizableInterface* InTarget) override;
    /**End IPhysicsTaskRegistrar Interface */
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

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UARMagneticTickObject> AttractionTickClass;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UARMagneticTickObject> RepulsionTickClass;

};
