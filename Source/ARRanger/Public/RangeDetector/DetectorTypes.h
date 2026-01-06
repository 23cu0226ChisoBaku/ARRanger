#pragma once

#include "CoreMinimal.h"

#include "DetectorTypes.generated.h"

class AActor;
class UInterface;
class UPrimitiveDetectorData;

UENUM(BlueprintType)
enum struct EDetectorTargetType : uint8
{
  Actor,      // AActorまたはその子クラス
  Interface,  // UEインターフェイスを継承したAActor

  INVALID UMETA(Hidden)
};

/**
 * @brief 探知ターゲット情報
 */
USTRUCT(BlueprintType)
struct FDetectorTargetInfo
{
  GENERATED_BODY()

  // ターゲットタイプ
  UPROPERTY(EditDefaultsOnly)
  EDetectorTargetType Type = EDetectorTargetType::Actor;

  // ターゲットActorのActorサブクラス
  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Target Actor Class"))
  TSubclassOf<AActor> TargetActor = AActor::StaticClass();

  // ターゲットActorが継承したインターフェイスサブクラス
  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Target UInterface Class"))
  TSubclassOf<UInterface> TargetInterface = UInterface::StaticClass();
};

USTRUCT(BlueprintType)
struct FDetectorAssetEntry
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (DisplayName = "AssetPtr"))
  TObjectPtr<UPrimitiveDetectorData> DetectorData = nullptr;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|TargetInfo", meta = (EditCondition = "DetectorData != nullptr", EditConditionHides))
  FDetectorTargetInfo TargetInfo;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (EditCondition = "DetectorData != nullptr", EditConditionHides))
  int32 Priority = 0;

};