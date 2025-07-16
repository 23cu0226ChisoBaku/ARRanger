#include "CustomRenderPipeline/MangaExaggerationStyleSVExtension.h"

#include "CustomRenderPipeline/CustomRPWorldSubsystem.h"

namespace AR
{
namespace CustomRP
{
  FMangaExaggerationStyleSVExtension::FMangaExaggerationStyleSVExtension(const FAutoRegister& AutoRegister, UCustomRPWorldSubsystem* Subsystem)
    : FSceneViewExtensionBase(AutoRegister)
    , m_CRPSubsystem{Subsystem}
  {

  }

  void FMangaExaggerationStyleSVExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
  {
    FMatrix newProjectionMatrix = InView.ViewMatrices.GetProjectionMatrix();

    FVector camLocation = InView.ViewLocation;
    FRotator viewRotation = FRotator{InView.ViewRotation.Pitch, InView.ViewRotation.Yaw + 10.0, InView.ViewRotation.Roll};
    FVector camDirection = viewRotation.Vector();
    FMatrix viewMatrix = FLookAtMatrix44d(camLocation, camLocation + camDirection, FVector::UpVector);
    
    const FVector viewForward = -viewMatrix.GetUnitAxis(EAxis::X);
    const FVector viewUp = viewMatrix.GetUnitAxis(EAxis::Z);
    const FRotator viewRotator = FRotationMatrix44d::MakeFromXZ(viewForward, viewUp).Rotator();
    
    InView.ViewMatrices.UpdateViewMatrix(camLocation, viewRotator);
    InView.UpdateViewMatrix();
    newProjectionMatrix.M[0][0] *= 0.6;
    newProjectionMatrix.M[1][1] *= 1.5;
    // newProjectionMatrix.M[0][2] += FMath::Sin(m_CRPSubsystem->GetWorld()->TimeSeconds * 10.0f) *0.05f;
    // newProjectionMatrix.M[1][2] += FMath::Cos(m_CRPSubsystem->GetWorld()->TimeSeconds * 12.0f) *0.05f;
    InView.UpdateProjectionMatrix(newProjectionMatrix);
  }

  void FMangaExaggerationStyleSVExtension::Invalidate()
  {
    m_CRPSubsystem = nullptr;
  }
}
}