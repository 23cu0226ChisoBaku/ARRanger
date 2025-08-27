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

USTRUCT(BlueprintType)
struct FDetectorAssetEntry
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data", meta = (DisplayName = "AssetPtr"))
  TObjectPtr<UPrimitiveDetectorData> DetectorData;

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector|Data")
  int32 Priority;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API URangeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URangeDetectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

  UPROPERTY(EditDefaultsOnly, Category = "RangeDetector", meta = (DisplayName = "DataAssetEntry"))
  TArray<FDetectorAssetEntry> DetectorAssetEntries;

private:
  TArray< TPimplPtr< ARRanger::Detector::FRangeDetector > > m_rangeDetectorInsts;
		
};
