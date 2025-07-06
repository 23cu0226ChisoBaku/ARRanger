#pragma once

#ifndef _MD_MESH_CAPTURE_HELPER_LIBRARY_
#define _MD_MESH_CAPTURE_HELPER_LIBRARY_

struct FProcMeshTangent;
struct FMDMeshVertexTangent;

namespace MotionDiff
{
  struct FMeshCaptureHelperLibrary
  {
    static void ConvertToProcMeshTangent(const TArray<FMDMeshVertexTangent>& InMDVertexTangent, TArray<FProcMeshTangent>& OutPMCVertexTangent);
    
    private:
      FMeshCaptureHelperLibrary() = delete;
      ~FMeshCaptureHelperLibrary() = delete;
  };
}

#endif // _MD_MESH_CAPTURE_HELPER_LIBRARY_