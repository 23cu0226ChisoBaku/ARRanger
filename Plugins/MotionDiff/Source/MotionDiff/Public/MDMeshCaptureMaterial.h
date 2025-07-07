// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MDMeshCaptureMaterial.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMDMeshCaptureMaterial
{
  GENERATED_BODY()

  MOTIONDIFF_API friend bool operator==(const FMDMeshCaptureMaterial& Lhs, const FMDMeshCaptureMaterial& Rhs);
  MOTIONDIFF_API friend bool operator!=(const FMDMeshCaptureMaterial& Lhs, const FMDMeshCaptureMaterial& Rhs);
  MOTIONDIFF_API friend bool operator==(const FMDMeshCaptureMaterial& Lhs, const UMaterialInterface& Rhs);
  MOTIONDIFF_API friend bool operator!=(const FMDMeshCaptureMaterial& Lhs, const UMaterialInterface& Rhs);
  MOTIONDIFF_API friend bool operator==(const UMaterialInterface& Lhs, const FMDMeshCaptureMaterial& Rhs);
  MOTIONDIFF_API friend bool operator!=(const UMaterialInterface& Lhs, const FMDMeshCaptureMaterial& Rhs);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionDiff|Material")
  TObjectPtr<UMaterialInterface> Material;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionDiff|Material")
  FName MaterialSymbolName;

  
};
