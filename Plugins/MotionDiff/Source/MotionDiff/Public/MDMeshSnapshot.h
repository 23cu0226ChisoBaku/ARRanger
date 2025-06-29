// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDMeshSnapshot.generated.h"


USTRUCT()
struct MOTIONDIFF_API FMDMeshVertexInfo
{
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere)
  FVector3f LocalMeshVertexPosition;

  int32 MeshVertexIndex;
};



USTRUCT(BlueprintType)
struct MOTIONDIFF_API FMDMeshSnapshot
{
  GENERATED_BODY()

  void Reset();

  UPROPERTY(VisibleAnywhere)
  TArray<FMDMeshVertexInfo> MeshVerticesInfo; 

  UPROPERTY(VisibleAnywhere)
  FName SnapshotName;

  UPROPERTY(VisibleAnywhere)
  bool bIsValid;
};
