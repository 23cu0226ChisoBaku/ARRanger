#include "PlayerComponents/LockOnComponent.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "RangeDetector/Core/RangeDetector.h"
#include "Enemy/Enemy_Zako.h"
#include "Kismet/GameplayStatics.h"
#include "RangeDetector/Utils/RangeDetectorHelper.h"

ULockOnComponent::ULockOnComponent()
  : MaxLockOnDistance{1500.0f}
  , LockedOnTarget{nullptr}
  , PlayerControllerWeak{nullptr}
  , DetectorRootComponent{nullptr}
  , m_bIsLockedOn{false}
  , m_lockOnTargetDetector{nullptr}
{
  PrimaryComponentTick.bCanEverTick = true;
  PrimaryComponentTick.TickGroup = TG_PrePhysics;

}

void ULockOnComponent::BeginPlay()
{
  Super::BeginPlay();

  APawn* ownerPawn = ::Cast<APawn>(GetOwner());
  check(ownerPawn != nullptr);
  
  if (ownerPawn != nullptr)
  {
    APlayerController* ownerPlayerController = ::Cast<APlayerController>(ownerPawn->GetController());
    check(ownerPlayerController != nullptr);

    if (ownerPlayerController != nullptr)
    {
      PlayerControllerWeak = ownerPlayerController;
    }
  }

  DetectorRootComponent = GetOwner()->GetRootComponent();

  // Editorで設定したDetectorEntryで範囲探知を初期化する
  SetupDetector(DetectorEntry);

}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // ロックオン情報を収集する
  {
    // カメラ情報
    FVector cameraPos{};
    FRotator cameraRot{};
    GatherCameraInfo(/**Out */cameraPos, /**Out */cameraRot);
  
    // ロックオン可能なターゲット
    TArray<AActor*> targets{};
    // ロックオン情報を送る
    if (GatherTargets(/**Out */targets) > 0)
    {
      if (OnLockOnDataUpdateEvent.IsBound())
      {
        OnLockOnDataUpdateEvent.Broadcast(cameraPos, cameraRot, targets);
      }
    }
  }
}

void ULockOnComponent::ToggleLockOn()
{
  if (m_bIsLockedOn)
  {
    // ロックオンを解除
    LockedOnTarget = nullptr;
    SetIsLockedOn(false);
  }
  else
  {
    AEnemy_Zako* Candidate = nullptr;
    if (Candidate)
    {
      // プレイヤーから見えていればロックオン開始
      LockedOnTarget = Candidate;
      SetIsLockedOn(true);
      UE_LOG(LogTemp, Warning, TEXT("LockOn: Locked on %s"), *Candidate->GetName());      
    }
  }
}

void ULockOnComponent::SwitchTargetRight()
{
  SwitchTarget(true);
}

void ULockOnComponent::SwitchTargetLeft()
{
  SwitchTarget(false);
}

void ULockOnComponent::SwitchTarget(bool bRight)
{
  // ロックオン中でない、またはプレイヤーがいなければ処理しない
  if (!m_bIsLockedOn || !LockedOnTarget.IsValid())
  {
    return;
  }

  // ワールドの敵を取得(To Do：一定範囲内の敵を取得するように修正)
  TArray<AActor*> Enemies;
  UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Enemy"), Enemies);

  // 敵が複数体いなければ処理しない
  if (Enemies.Num() <= 1)
  {
    return;
  }

  // 敵がいなければ処理しない
  int32 CurrentIndex = Enemies.IndexOfByKey(LockedOnTarget);
  if (CurrentIndex == INDEX_NONE)
  {
    return;
  }

  const FVector MyLocation = GetOwner()->GetActorLocation();
  const int32 EnemyCount = Enemies.Num();
  int32 Index = CurrentIndex;
  int32 Checked = 0;

  while (Checked < EnemyCount)
  {
    // その他の敵を判定
    Index = bRight ? (Index + 1) % EnemyCount : (Index - 1 + EnemyCount) % EnemyCount;

    // 現在の敵になったら処理をやめる
    if (Index == CurrentIndex)
    {
      break;
    }

    AActor* Candidate = Enemies[Index];
    if (!Candidate)
    {
      Checked++;
      continue;
    }

    float Distance = FVector::Dist(MyLocation, Candidate->GetActorLocation());
    if (Distance <= MaxLockOnDistance)
    {
      LockedOnTarget = Cast<AEnemy_Zako>(Candidate);
      return;
    }

    Checked++;
  }
}

void ULockOnComponent::GatherCameraInfo(FVector& OutCameraPos, FRotator& OutCameraRot)
{
  OutCameraPos = FVector{0.0, 0.0, 0.0};
  OutCameraRot = FRotator{0.0, 0.0, 0.0};

  if (PlayerControllerWeak.IsValid())
  {
    PlayerControllerWeak->GetPlayerViewPoint(OutCameraPos, OutCameraRot);
  }
}

int32 ULockOnComponent::GatherTargets(TArray<AActor*>& OutTargets)
{
  OutTargets.Reset();

  FRangeDetectorEvaluationParameter param = 
  {
    .World = GetWorld(),
    .OriginActor = GetOwner(),
    .OriginSceneComp = DetectorRootComponent.IsValid() ? DetectorRootComponent.Get() : GetOwner()->GetRootComponent()
  };

  // ロックオン可能なActorを探知する
  if (m_lockOnTargetDetector.IsValid())
  {
    m_lockOnTargetDetector->Evaluate(param);
    OutTargets = m_lockOnTargetDetector->GetEvaluatedResult().DetectedActors;
  }

  return OutTargets.Num();
}

void ULockOnComponent::SetupDetector(const FDetectorAssetEntry& InDetectorEntry)
{
  // 古いDetectorを解放する
  if (m_lockOnTargetDetector != nullptr)
  {
    m_lockOnTargetDetector.Reset();
  }

  if (InDetectorEntry.DetectorData != nullptr)
  {
    m_lockOnTargetDetector = ::MakePimpl<ARRanger::Detector::FRangeDetector>(*InDetectorEntry.DetectorData, InDetectorEntry.Priority);
  }

  // フィルターを追加
  if (m_lockOnTargetDetector.IsValid())
  {
    const FDetectorTargetInfo& targetInfoRef = InDetectorEntry.TargetInfo;

    FRangeDetectorFilterData filter{};
    filter.Type = targetInfoRef.Type;

    switch (targetInfoRef.Type)
    {
      case EDetectorTargetType::Actor:
      {
        filter.FilterClass = targetInfoRef.TargetActor; 
      }
      break;

      case EDetectorTargetType::Interface:
      {
        filter.FilterClass = targetInfoRef.TargetInterface;
      }
      break;
    }

    m_lockOnTargetDetector->AddFilter(::MoveTemp(filter));

    // 起動する
    m_lockOnTargetDetector->SetEnable(true);
  }
}