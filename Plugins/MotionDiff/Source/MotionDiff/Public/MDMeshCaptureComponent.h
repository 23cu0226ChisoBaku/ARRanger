// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MDMeshCaptureComponent.generated.h"

UENUM()
enum struct EMDMeshCaptureDestroyMode : uint8
{
  ElapsedTime,

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

  UPROPERTY(EditAnywhere, meta = (EditCondition = "DestroyMode == EMDMeshCaptureDestroyMode::ElapsedTime"))
  FMDMeshCapture_ElapseModeParameter ElapsedTimeParam;
};











UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOTIONDIFF_API UMDMeshCaptureComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UMDMeshCaptureComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:	
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
  UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
  FMDMeshCaptureParameters m_captureParam;



    
};
