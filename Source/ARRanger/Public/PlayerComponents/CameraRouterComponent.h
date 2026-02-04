#pragma once

#include "Components/ActorComponent.h"

#include "Player/CameraRigType.h"

#include "CameraRouterComponent.generated.h"

/**
 * @brief Router class for transfroming event to BP
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API UCameraRouterComponent : public UActorComponent
{
	GENERATED_BODY()

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraRigChangedEvent, ECameraRigType, InType);

public:
  UPROPERTY(BlueprintAssignable)
  FOnCameraRigChangedEvent OnCameraRigChanged;

	// Sets default values for this component's properties
	UCameraRouterComponent();

  UFUNCTION(BlueprintCallable, Category = "Camera")
  void ChangeCameraRig(ECameraRigType InType);
		
};
