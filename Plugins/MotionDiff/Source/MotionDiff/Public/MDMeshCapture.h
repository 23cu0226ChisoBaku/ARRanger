// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MDMeshCapture.generated.h"

struct FMDMeshSnapshot;
/**
 * 
 */
UCLASS()
class MOTIONDIFF_API UMDMeshCapture : public UObject
{
	GENERATED_BODY()
	
  private:
    TArray<FTransform> m_curtCapturedTransforms;
};
