/**
 * @file ARMagneticTickObject.h
 * @author MAI ZHICONG
 * @brief Base class of Magnetism tick object
 * 磁力Tickオブジェクトの基底クラス
 */

#pragma once

#include "Physics/Core/ARPhysicsTickObject.h"

#include "ARMagneticTickObject.generated.h"

/**前方宣言 */
class IARMagnetizableInterface;

#define ARPHYSICS_API ARRANGER_API

UCLASS(MinimalAPI, Abstract)
class UARMagneticTickObject : public UARPhysicsTickObject
{
	GENERATED_BODY()

  public:
    ARPHYSICS_API UARMagneticTickObject();

    /**
     * @brief 磁力影響を与えるオブジェクトを登録する
     * @param InMagnetizedObject 
     */
    ARPHYSICS_API void RegisterAffectedMagnetizedObject(IARMagnetizableInterface* InMagnetizedObject);

    void SetTargetObject(IARMagnetizableInterface* InTarget) { Target = InTarget;}
    bool IsSameTarget(const IARMagnetizableInterface* InTarget) const { return Target == InTarget; }
    bool IsTargetValid() const { return Target != nullptr; } 
    IARMagnetizableInterface* GetTarget() const { return Target; }

  protected:
    /**Start UARPhysicsTickObject interface */
    ARPHYSICS_API virtual void OnPostTickObject() override;
    /**End UARPhysicsTickObject interface */

  protected:

    /**磁力を受けるターゲット */
    IARMagnetizableInterface* Target;

    /**ターゲットに影響を与えるオブジェクト群 */
    TArray<IARMagnetizableInterface*> AffectedMagnetizedObjects;

};

#undef ARPHYSICS_API
