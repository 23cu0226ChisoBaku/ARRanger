#include "PlayerComponents/CameraRouterComponent.h"

UCameraRouterComponent::UCameraRouterComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UCameraRouterComponent::ChangeCameraRig(ECameraRigType InType)
{
  OnCameraRigChanged.Broadcast(InType);
}
