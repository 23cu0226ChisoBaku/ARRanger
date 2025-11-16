#pragma once

#include "Physics/TickObjects/Magnetic/ARMagneticTickObject.h"

#include "ARMagneticAttractionTickObject.generated.h"

/**
 * @brief 引力TickObject
 */
UCLASS(Blueprintable, BlueprintType)
class UARMagneticAttractionTickObject : public UARMagneticTickObject
{
	GENERATED_BODY()
  
  protected:     
    /**Start UARPhysicsTickObject interface */ 
    ARRANGER_API virtual void OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result) override;
    ARRANGER_API virtual void OnPostTickObject() override;
    /**End UARPhysicsTickObject interface */
};
