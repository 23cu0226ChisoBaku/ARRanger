// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "MeshCapture/MDMeshCapture.h"
#include "MDMeshCaptureFactory.h"
#include "MDMeshCaptureMaterial.h"

#include "MDMeshCaptureComponent.generated.h"

namespace MotionDiff
{
  struct FMDMeshCaptureShutter;
}


UENUM()
enum struct EMDMeshCaptureDestroyMode : uint8
{
  None = 0,     // 消滅ない
  ElapsedTime,  // 時間による消滅

};

UENUM()
enum struct EMDMeshCaptureShutterMode : uint8
{
  Default = 0,    
  IntervalBase,
};

USTRUCT()
struct MOTIONDIFF_API FMDMeshCapture_ElapseModeParameter
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, meta = (DisplayName = "Time Interval"))
  float IntervalTime;
};


/**
 * Mesh capture parameter container
 * 
 * メッシュをキャプチャーする変数構造体
 */
USTRUCT(BlueprintType)
struct MOTIONDIFF_API FMDMeshCaptureParameters
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsCapturing == true", EditConditionHides, ClampMin = 0.0f))
  float CaptureInterval;

  UPROPERTY(EditAnywhere)
  bool bIsCapturing;
  
  UPROPERTY(EditAnywhere, meta = (EditCondition = "bIsCapturing == true", EditConditionHides))
  bool bShouldDestroy;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "bShouldDestroy == true", EditConditionHides))
  EMDMeshCaptureDestroyMode DestroyMode;

  UPROPERTY(EditAnywhere, meta = (EditCondition = "DestroyMode == EMDMeshCaptureDestroyMode::ElapsedTime", EditConditionHides))
  FMDMeshCapture_ElapseModeParameter ElapsedTimeParam;
};








// 前方宣言
class UMDMeshCapture;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOTIONDIFF_API UMDMeshCaptureComponent : public UActorComponent
{
  GENERATED_BODY()

  public:
    // Sets default values for this component's properties
    UMDMeshCaptureComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginDestroy() override;

  protected:
    // Called when the game starts
    virtual void BeginPlay() override;

  public:	
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    template<typename MeshComponentType>
    void StartCapture(MeshComponentType* MeshComponent)
    {
      if (m_captureInstance != nullptr)
      {
        m_captureInstance->Reset();
      }

      m_captureInstance = FMDMeshCaptureFactory::CreateCapture(this, MeshComponent);
    }

  // Editor専用関数
  // Override 
  #if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
  #endif

  // Private
  #if WITH_EDITOR
  private:
    void OnCaptureParameterChanged();
  #endif

  private:
    MotionDiff::FMDMeshCaptureShutter* CreateShutter();
    void DestroyShutter(MotionDiff::FMDMeshCaptureShutter* ShutterPtr);

  private:
    UPROPERTY(EditAnywhere, Category = "Mesh Capture", meta = (AllowPrivateAccess = "true"))
    bool bUseFirstMeshAttachToThis;

    UPROPERTY(EditAnywhere, Category = "Mesh Capture", meta = (AllowPrivateAccess = "true"))
    TArray<FMDMeshCaptureMaterial> SnapshotMaterials;

    UPROPERTY(EditAnywhere, Category = "Mesh Capture", meta = (AllowPrivateAccess = "true"))
    FMDMeshCaptureParameters CaptureParam;

    UPROPERTY()
    TObjectPtr<UMDMeshCapture> m_captureInstance;

    MotionDiff::FMDMeshCaptureShutter* m_shutterInstance;

    EMDMeshCaptureDestroyMode m_currentCaptureDestroyMode = EMDMeshCaptureDestroyMode::None;

};
