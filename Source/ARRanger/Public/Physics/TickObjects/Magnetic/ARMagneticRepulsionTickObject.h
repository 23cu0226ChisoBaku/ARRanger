#pragma once

#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"

#include "ARMagneticRepulsionTickObject.generated.h"

/**
 * @brief 斥力TickObject
 */
UCLASS(Blueprintable, BlueprintType)
class UARMagneticRepulsionTickObject : public UARMagneticTickObject
{
	GENERATED_BODY()

  protected:
    /**Start UARPhysicsTickObject interface */
    ARRANGER_API virtual void OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result) override;
    ARRANGER_API virtual void OnPostTickObject() override;
    /**End UARPhysicsTickObject interface */
};
