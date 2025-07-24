// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MDMeshCapture.h"
#include "MDStaticMeshCapture.generated.h"

class UStaticMeshComponent;


UCLASS()
class MOTIONDIFF_API UMDStaticMeshCapture : public UMDMeshCapture
{
	GENERATED_BODY()

  public:
    UMDStaticMeshCapture(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    virtual void CaptureMesh(UMeshComponent* MeshComponent) override;
    virtual void Reset() override;
    virtual void ShowSnapshots() override;
    virtual void HideSnapshots() override;
    virtual FString GetCaptureName() const override;

  protected:
    virtual void SnapshotMesh(FMDMeshSnapshot& Snapshot, const int32 LODIndex) override;

#if WITH_EDITOR
  private:
    // FIXME: リアルタイムで修正しても影響がないらしい
    void EnableCPUAccess(UStaticMeshComponent* StaticMeshComponent);
#endif

  private:
    FMeshDescription* GetMeshDescription(const int32 LODIndex);
    const FMeshDescription* GetMeshDescription(const int32 LODIndex) const;

  private:
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> m_staticMeshComp;
};
