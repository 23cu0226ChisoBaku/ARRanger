// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"

#include "RangeDetectorComponent.generated.h"


/**
 * Forward declaration
 */
struct FRangeDetectorEvaluationResult;

namespace ARRanger
{
  
  namespace Detector
  {
    class FRangeDetector;
  }
  
}

class UPrimitiveDetectorData;

#define UE_API ARRANGER_API

UENUM(BlueprintType)
enum struct EDetectorTargetType : uint8
{
  /**アクター */
  Actor,

  /**アンリアル インターフェイス */
  Interface,
};

USTRUCT(BlueprintType)
struct FDetectorTarget
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly)
  EDetectorTargetType TargetType = EDetectorTargetType::Actor;

  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Target Actor Class"))
  TSubclassOf<AActor> TargetActor = nullptr;

  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Target UInterface Class"))
  TSubclassOf<UInterface> TargetInterface = nullptr;
};

USTRUCT(BlueprintType)
struct FDetectorAssetEntry
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (DisplayName = "AssetPtr"))
  TObjectPtr<UPrimitiveDetectorData> DetectorData = nullptr;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Target", meta = (EditCondition = "DetectorData != nullptr", EditConditionHides))
  FDetectorTarget Target;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (EditCondition = "DetectorData != nullptr", EditConditionHides))
  int32 Priority = 0;

};

// Use USceneComponent to attach this to mesh component
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URangeDetectorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UE_API URangeDetectorComponent();

protected:
	// Called when the game starts
	UE_API virtual void BeginPlay() override;

public:	
  /**Start UActorComponent Interface */
	// Called every frame
	UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  UE_API virtual void OnUnregister() override;
  /**End UActorComponent Interface */

  UE_API void AddNewDetector(const FDetectorAssetEntry& Entry);

  UE_API void RemoveDetector(const UPrimitiveDetectorData* RangeData);

  UE_API bool HasRangeData(const UPrimitiveDetectorData* RangeData);

  UE_API int32 GetResultByRangeData(const UPrimitiveDetectorData* RangeData, FRangeDetectorEvaluationResult& OutResult) const;

  UE_API bool IsDetectorEmpty() const;

// Only for FComponentVisualizer Use
#if WITH_EDITOR

  UE_API void ED_DrawComponentVisualizer(class FPrimitiveDrawInterface* PDI) const;

  UE_API void ED_DrawWithLineBatchComp(class ULineBatchComponent* LineBatch) const;
  
#endif 

private:

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector", meta = (DisplayName = "DataAssetEntry", AllowPrivateAccess = "true"))
  TArray<FDetectorAssetEntry> DetectorAssetEntries;

  
  private:
  TArray< TPimplPtr< ARRanger::Detector::FRangeDetector > > m_rangeDetectorInsts;
  
public:

  #if WITH_EDITORONLY_DATA

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Debug", Transient)
  bool bDrawDebugRange = false;
  
  #endif // WITH_EDITORONLY_DATA
  
  uint8 bStopWhenOwnerDestroyed : 1;
};

#undef UE_API