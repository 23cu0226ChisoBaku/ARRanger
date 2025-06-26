// Fill out your copyright notice in the Description page of Project Settings.


#include "MDMeshCaptureComponent.h"
#include "MotionDiff/MotionDiffLogChannels.h"

// Sets default values for this component's properties
UMDMeshCaptureComponent::UMDMeshCaptureComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , CaptureParam{}
  , CapturedTrailMaterial{nullptr}
  , m_captureInstance{nullptr}
  , bUseFirstMeshAttachToThis{false}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UMDMeshCaptureComponent::BeginPlay()
{
	Super::BeginPlay();

  if (bUseFirstMeshAttachToThis)
  {
    const AActor* owner = GetOwner();
    bool bIsCreated = false;

    // スタティックメッシュ
    {
      TArray<UStaticMeshComponent*> ownedStaticMeshComps;
      owner->GetComponents<UStaticMeshComponent>(ownedStaticMeshComps);
      if (ownedStaticMeshComps.Num() != 0)
      {
        // 複数があっても最初に見つけたコンポーネントでしか初期化しない
        m_captureInstance = FMDMeshCaptureFactory::CreateCapture(ownedStaticMeshComps[0]);
        bIsCreated = true;
      }
    }

    // スケルタルメッシュ
    if (!bIsCreated)
    {
      TArray<USkeletalMeshComponent*> ownedSkeletalMeshComps;
      owner->GetComponents<USkeletalMeshComponent>(ownedSkeletalMeshComps);
      if (ownedSkeletalMeshComps.Num() != 0)
      {
        // 複数があっても最初に見つけたコンポーネントでしか初期化しない
        m_captureInstance = FMDMeshCaptureFactory::CreateCapture(ownedSkeletalMeshComps[0]);
        bIsCreated = true;
      }
    }

    // 見つからない時ログを出す
    if (!bIsCreated)
    {
      UE_LOG(LogMotionDiff, Warning, TEXT("Can not find any useable mesh component in Actor[%s]"), *GetNameSafe(owner));
    }

    // キャプチャしないもしくは破壊しない場合はTickしないようにする
    if (!CaptureParam.bIsCapturing || !CaptureParam.bShouldDestroy)
    {
      SetComponentTickEnabled(false);
    }

    m_currentCaptureDestroyMode = CaptureParam.DestroyMode;
  }
}


// Called every frame
void UMDMeshCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

// Editor専用関数
#if WITH_EDITOR
  void UMDMeshCaptureComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
  {
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName changedPropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (changedPropertyName == GET_MEMBER_NAME_CHECKED(UMDMeshCaptureComponent, CaptureParam))
    {
      OnCaptureParameterChanged();
    }
  }

  void UMDMeshCaptureComponent::OnCaptureParameterChanged()
  {
    SetComponentTickEnabled(CaptureParam.bIsCapturing && CaptureParam.bShouldDestroy);
    if (m_currentCaptureDestroyMode != CaptureParam.DestroyMode)
    {

      m_currentCaptureDestroyMode = CaptureParam.DestroyMode;
    }
  }


#endif



