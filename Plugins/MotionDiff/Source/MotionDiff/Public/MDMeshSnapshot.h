// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDMeshSnapshot.generated.h"

USTRUCT(BlueprintType)
struct FMDMeshVertexTangent
{
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere)
  FVector TangentX;

  UPROPERTY(VisibleAnywhere)
  bool bFlipTangentY;
};

USTRUCT(BlueprintType)
struct MOTIONDIFF_API FMDMeshVertexBuffers
{
  GENERATED_BODY()

  void Reset();

  UPROPERTY(VisibleAnywhere)
  TArray<FVector> Positions;

  UPROPERTY(VisibleAnywhere)
  TArray<int32> Triangles;

  UPROPERTY(VisibleAnywhere)
  TArray<FVector> Normals;

  UPROPERTY(VisibleAnywhere)
  TArray<FVector2D> UVs0;

  UPROPERTY(VisibleAnywhere)
  TArray<FVector2D> UVs1;

  UPROPERTY(VisibleAnywhere)
  TArray<FVector2D> UVs2;

  UPROPERTY(VisibleAnywhere)
  TArray<FVector2D> UVs3;

  UPROPERTY(VisibleAnywhere)
  TArray<FLinearColor> Colors;

  UPROPERTY(VisibleAnywhere)
  TArray<FMDMeshVertexTangent> Tangents;

};



USTRUCT(BlueprintType)
struct MOTIONDIFF_API FMDMeshSnapshot
{
  GENERATED_BODY()

  void Reset();

  UPROPERTY(VisibleAnywhere)
  FMDMeshVertexBuffers MeshVertexBuffers;

  UPROPERTY(VisibleAnywhere)
  FName SnapshotName;

  UPROPERTY(VisibleAnywhere)
  bool bIsValid;

  UPROPERTY(VisibleAnywhere)
  int8 LODIndex;              // 無効値として-1を入れるようにint8を使用
};
