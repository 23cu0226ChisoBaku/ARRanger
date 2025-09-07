// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeDetector/GameFramework/RangeDetectorComponent.h"

#include "RangeDetector/Core/RangeDetector.h"
#include "RangeDetector/Core/PrimitiveDetectorData.h"

namespace
{
  void FilterActorByClass(TArray<TObjectPtr<AActor>>& OutResult, TSubclassOf<AActor> ActorClass);
  void FilterActorByInterface(TArray<TObjectPtr<AActor>>& OutResult, TSubclassOf<UInterface> InterfaceClass);
}

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

    detectorInst->Enable();

    m_rangeDetectorInsts.Emplace(::MoveTemp(detectorInst));
  }
}


// Called every frame
void URangeDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
  check(GetWorld() != nullptr);

  FRangeDetectorEvaluationParameter evaluationParam
  {
    .World = GetWorld(),
    .OriginActor = GetOwner()  
  };

	for (int32 idx = 0; idx < m_rangeDetectorInsts.Num(); ++idx)
  {
    const TPimplPtr< ARRanger::Detector::FRangeDetector >& detector = m_rangeDetectorInsts[idx];
    if (detector.IsValid())
    {
      FRangeDetectorEvaluationResult result{};
      detector->EvaluateDetector(evaluationParam, result);

      // TODO Bind entry to detector
      check(idx < DetectorAssetEntries.Num());
      
      using enum EDetectorTargetType;
      switch (DetectorAssetEntries[idx].Target.TargetType)
      {
        case Actor:
        {
          FilterActorByClass(result.DetectedActors, DetectorAssetEntries[idx].Target.TargetActor);
        }
        break;

        case Interface:
        {
          FilterActorByInterface(result.DetectedActors, DetectorAssetEntries[idx].Target.TargetInterface);
        }
        break;
      }

      // TODO For debug purpose
#if WITH_EDITOR

      UE_LOG(LogTemp, Warning, TEXT("Hit count: %d"), result.DetectedActors.Num());
      if (GEngine != nullptr)
      {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Hit count: %d"), result.DetectedActors.Num()));
      }
      detector->DebugDrawRange(GetOwner()->GetActorLocation(), FColor::Red);

#endif // WITH_EDITOR
    }
  }
}

namespace
{
  void FilterActorByClass(TArray<TObjectPtr<AActor>>& OutResult, TSubclassOf<AActor> ActorClass)
  {
    if (ActorClass == nullptr)
    {
      return;
    }

    int32 idx = 0;
    while (idx < OutResult.Num())
    {
      AActor* actor = OutResult[idx];
      // Remove invalid actor or actor that is not implemented ActorClass
      if ((actor == nullptr) || !actor->GetClass()->IsChildOf(ActorClass))
      {
        OutResult.RemoveAt(idx);
        continue;
      }

      idx++;
    }
  }

  void FilterActorByInterface(TArray<TObjectPtr<AActor>>& OutResult, TSubclassOf<UInterface> InterfaceClass)
  {
    if (InterfaceClass == nullptr)
    {
      return;
    }

    int32 idx = 0;
    while (idx < OutResult.Num())
    {
      AActor* actor = OutResult[idx];
      // Remove invalid actor or actor that is not implemented InterfaceClass
      if ((actor == nullptr) || !actor->GetClass()->ImplementsInterface(InterfaceClass))
      {
        OutResult.RemoveAt(idx);
        continue;
      }

      idx++;
    }
  }
}
