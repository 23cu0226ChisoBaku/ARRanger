// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeDetector/GameFramework/RangeDetectorComponent.h"

#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

// Sets default values for this component's properties
URangeDetectorComponent::URangeDetectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URangeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

  for (const auto& entry : DetectorAssetEntries)
  {
    check(entry.DetectorData != nullptr);

    TPimplPtr< ARRanger::Detector::FRangeDetector > detectorInst = 
      ::MakePimpl< ARRanger::Detector::FRangeDetector >(entry.DetectorData, entry.Priority);

    m_rangeDetectorInsts.Emplace(::MoveTemp(detectorInst));
  }
}


// Called every frame
void URangeDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 idx = 0; idx < m_rangeDetectorInsts.Num(); ++idx)
  {
    const TPimplPtr< ARRanger::Detector::FRangeDetector >& detector = m_rangeDetectorInsts[idx];
    if (detector.IsValid())
    {
      TArray<AActor*> targets{};
      (void)detector->EvaluateDetector(targets);

#if WITH_EDITOR
      detector->DebugDrawRange(GetOwner()->GetActorLocation(), FColor::Red);

#endif // WITH_EDITOR
    }
  }
}

