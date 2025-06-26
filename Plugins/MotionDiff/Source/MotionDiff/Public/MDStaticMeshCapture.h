// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDMeshCapture.h"
#include "MDStaticMeshCapture.generated.h"

/**
 * 
 */
UCLASS()
class MOTIONDIFF_API UMDStaticMeshCapture : public UMDMeshCapture
{
	GENERATED_BODY()

  public:
    virtual void CaptureMesh(UMeshComponent* MeshComponent) override;
    virtual void Reset() override;
    virtual void ShowSnapshots() override;
    virtual void HideSnapshots() override;
	
};
