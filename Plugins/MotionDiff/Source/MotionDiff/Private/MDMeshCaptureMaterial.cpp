// Fill out your copyright notice in the Description page of Project Settings.


#include "MDMeshCaptureMaterial.h"

bool operator==(const FMDMeshCaptureMaterial& Lhs, const FMDMeshCaptureMaterial& Rhs)
{
  return Lhs.Material == Rhs.Material;
}

bool operator!=(const FMDMeshCaptureMaterial& Lhs, const FMDMeshCaptureMaterial& Rhs)
{
  return !(Lhs == Rhs);
}

bool operator==(const FMDMeshCaptureMaterial& Lhs, const UMaterialInterface& Rhs)
{
  return Lhs.Material == &Rhs;
}

bool operator!=(const FMDMeshCaptureMaterial& Lhs, const UMaterialInterface& Rhs)
{
  return !(Lhs == Rhs);
}

bool operator==(const UMaterialInterface& Lhs, const FMDMeshCaptureMaterial& Rhs)
{
  return &Lhs == Rhs.Material;
}

bool operator!=(const UMaterialInterface& Lhs, const FMDMeshCaptureMaterial& Rhs)
{
  return !(Lhs == Rhs);
}