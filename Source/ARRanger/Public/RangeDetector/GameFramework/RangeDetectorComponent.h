// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"

#include "RangeDetectorComponent.generated.h"


/**
 * Forward declaration
 */
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
  EDetectorTargetType TargetType;

  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Target Actor Class"))
  TSubclassOf<AActor> TargetActor;

  UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Target UInterface Class"))
  TSubclassOf<UInterface> TargetInterface;
};

USTRUCT(BlueprintType)
struct FDetectorAssetEntry
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (DisplayName = "AssetPtr"))
  TObjectPtr<UPrimitiveDetectorData> DetectorData;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Target", meta = (EditCondition = "DetectorData != nullptr", EditConditionHides))
  FDetectorTarget Target;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (EditCondition = "DetectorData != nullptr", EditConditionHides))
  int32 Priority;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class URangeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UE_API URangeDetectorComponent();

protected:
	// Called when the game starts
	UE_API virtual void BeginPlay() override;

public:	
	// Called every frame
	UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  UE_API void AddNewDetector(const FDetectorAssetEntry& Entry);

private:

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector", meta = (DisplayName = "DataAssetEntry"))
  TArray<FDetectorAssetEntry> DetectorAssetEntries;

private:
  TArray< TPimplPtr< ARRanger::Detector::FRangeDetector > > m_rangeDetectorInsts;
		
};

#undef UE_API