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

#define ARPHYSICS_API ARRANGER_API

USTRUCT()
struct FARMagneticTickObjectEntry
{
  GENERATED_BODY()

  UPROPERTY()
  TObjectPtr<UARMagneticTickObject> TickObject;

  TArray<TWeakInterfacePtr<IARMagnetizableInterface>> AffectedObjectInterfaces;

  void RegisterAffectedMagnetizedObject() const;
  ARPHYSICS_API friend bool operator==(const FARMagneticTickObjectEntry& Lhs, const FARMagneticTickObjectEntry& Rhs);
};

UCLASS()
class AARPhysicsTickProcessorActor : public AActor , public IPhysicsTaskRegistrar
{
	GENERATED_BODY()

  public:	
    ARPHYSICS_API AARPhysicsTickProcessorActor();

    /**Start IPhysicsTaskRegistrar Interface */
    ARPHYSICS_API void RegisterMagneticTask(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency) override;
    ARPHYSICS_API void UnregisterMagneticTask(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj) override;
    /**End IPhysicsTaskRegistrar Interface */

  protected:
    /**Start AActor interface */
    ARPHYSICS_API virtual void Tick(float DeltaTime) override;
    /**End AActor interface */

    void PreProcessARPhysicsTasks();
    void ProcessARPhysicsTasks(float DeltaTime);
    void PostProcessARPhysicsTasks();
  
  private:
    /**
     * @brief 磁力ターゲットの影響を与えるオブジェクトを登録する
     * 
     * @param InTarget ターゲット
     * @param InAffectedObj 影響を与えるオブジェクト
     * @param InRequestType 磁力タイプ（引力・斥力）
     * @param InFrequency Tick頻度（1回や常に実行する）
     */
    void RegisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj, EPhysicsRegistryType InRequestType, EPhysicsExecuteFrequency InFrequency);

    /**
     * @brief 磁力ターゲットの影響を与えるオブジェクトを解読する
     * @param InTarget ターゲット
     * @param InAffectedObj 影響を与えるオブジェクト
     */
    void UnregisterMagneticTarget(IARMagnetizableInterface* InTarget, IARMagnetizableInterface* InAffectedObj);

    /**
     * @brief キューにあるTickObjectをTickManagerに登録する
     */
    void RegisterQueuedTickObject();

    /**
     * @brief キューにあるTickObjectを解読する
     */
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

#undef ARPHYSICS_API
