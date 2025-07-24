#include "MDMeshCaptureHelperLibrary.h"

#include "MDMeshSnapshot.h"

#include "ProceduralMeshComponent.h"


namespace MotionDiff
{
  void FMeshCaptureHelperLibrary::ConvertToProcMeshTangent(const TArray<FMDMeshVertexTangent>& InMDVertexTangent, TArray<FProcMeshTangent>& OutPMCVertexTangent)
  {
    OutPMCVertexTangent.Reset(InMDVertexTangent.Num());
    for (int32 i = 0; i < InMDVertexTangent.Num(); ++i)
    {
      FProcMeshTangent convertedTangent{InMDVertexTangent[i].TangentX, InMDVertexTangent[i].bFlipTangentY};
      OutPMCVertexTangent.Add(convertedTangent);
    }
  
    check(OutPMCVertexTangent.Num() == InMDVertexTangent.Num());
  }
}