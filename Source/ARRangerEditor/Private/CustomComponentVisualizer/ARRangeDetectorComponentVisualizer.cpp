// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomComponentVisualizer/ARRangeDetectorComponentVisualizer.h"
#include "RangeDetector/GameFramework/RangeDetectorComponent.h"
#include "SceneManagement.h"

TSharedPtr<FComponentVisualizer> FARRangeDetectorComponentVisualizer::MakeInstance()
{
  return ::MakeShared<FARRangeDetectorComponentVisualizer>();
}

void FARRangeDetectorComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
  if (const URangeDetectorComponent* RDC = ::Cast<URangeDetectorComponent>(Component))
  {
    RDC->ED_DrawComponentVisualizer(PDI);
  }
}