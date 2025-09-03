// #pragma once

// #include "Core/CameraParameters.h"
// #include "ValueInterpolators/CriticalDamperValueInterpolator.h"
// #include "Math/CriticalDamper.h"

// #include "EnhancedCriticalDamperValueInterpolator.generated.h"

// /**
//  * An extension of CriticalDamperValueInterpolator
//  * 
//  * Edit by MAI ZHICONG in 2025/08/29
//  */
// UCLASS(meta = (DisplayName = "Enhanced Critical Damper"))
// class UEnhancedCriticalDamperValueInterpolator : public UCriticalDamperValueInterpolator
// {
//   GENERATED_BODY()

//   UE_DECLARE_CAMERA_VALUE_INTERPOLATOR()

// public:

//   /**Damping factor multiplier */
//   UPROPERTY(EditAnywhere, Category = "Damping")
//   FFloatCameraParameter DampingMultiplier = 1.0f;
// };