#pragma once

#include "Components/ActorComponent.h"
#include "RangeDetector/DetectorTypes.h"

#include "LockOnComponent.generated.h"

// 
class UPrimitiveDetectorData;
class APlayerController;
class AEnemy_Zako;

namespace ARRanger
{
namespace Detector
{
  // ARRanger::Detector::FRangeDetector
  class FRangeDetector;
} // namespace ARRanger::Detector
} // namespace ARRanger

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

  DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLockOnDataUpdated, const FVector&, const FRotator&, const TArray<AActor*>&);

public:
  ARRANGER_API ULockOnComponent();

  ARRANGER_API virtual void BeginPlay() override;

  ARRANGER_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  ARRANGER_API void SetLockOnRootComponent(USceneComponent* RootComponent);

  // ロックオン関数
  UFUNCTION(BlueprintCallable)
  ARRANGER_API void ToggleLockOn();

  // ターゲット切り替え(右)
  UFUNCTION(BlueprintCallable)
  ARRANGER_API void SwitchTargetRight();

  // ターゲット切り替え(左)
  UFUNCTION(BlueprintCallable)
  ARRANGER_API void SwitchTargetLeft();

  ARRANGER_API void SetupDetector(const FDetectorAssetEntry& InDetectorEntry);
  
  // ロックオンフラグをセット
  void SetIsLockedOn(bool IsLockedOn) { m_bIsLockedOn = IsLockedOn; }
  
  // ロックオン中フラグを取得
  bool GetIsLockedOn() { return m_bIsLockedOn; }

  // ロックオン中の敵を取得
  AEnemy_Zako* GetLockedOnTarget() const { return LockedOnTarget.IsValid() ? LockedOnTarget.Get() : nullptr; }

private:
  // ターゲット切り替え処理の関数
  void SwitchTarget(bool IsRight);
  
  void GatherCameraInfo(FVector& OutCameraPos, FRotator& OutCameraRot);
  int32 GatherTargets(TArray<AActor*>& OutTargets);

public:
  FOnLockOnDataUpdated OnLockOnDataUpdateEvent;

private:
  // ロックオン可能距離
  UPROPERTY(EditAnywhere, Category = "LockOn")
  float MaxLockOnDistance;

  UPROPERTY(EditAnywhere, Category = "LockOn")
  FDetectorAssetEntry DetectorEntry;

  // ターゲットを取得
  TWeakObjectPtr<class AEnemy_Zako> LockedOnTarget;

  TWeakObjectPtr<APlayerController> PlayerControllerWeak;

  TWeakObjectPtr<USceneComponent> DetectorRootComponent;

  // ロックオン中かどうか
  // TODO Deprecate this
  bool m_bIsLockedOn;

  TPimplPtr<ARRanger::Detector::FRangeDetector> m_lockOnTargetDetector;

};