#pragma once

#ifndef _MD_MESH_GENERATOR_
#define _MD_MESH_GENERATOR_

struct FMDMeshSnapshot;
class AActor;
class UWorld;
class UMaterialInterface;

namespace MotionDiff
{
  struct FMDMeshGenerator
  {
    static AActor* GeneratePMCActorBySnapshot(UWorld* World, const FMDMeshSnapshot& Snapshot, const int32 SectionNum, const TArray<UMaterialInterface*>& Materials);
    static AActor* GenerateStaticMeshActorBySnapshot(UWorld* World, const FMDMeshSnapshot& Snapshot, const int32 SectionNum, const TArray<UMaterialInterface*>& Materials);
    
    FMDMeshGenerator() = delete;
    ~FMDMeshGenerator() = delete;
  
  };
} // namespace MotionDiff

#endif // _MD_MESH_GENERATOR_