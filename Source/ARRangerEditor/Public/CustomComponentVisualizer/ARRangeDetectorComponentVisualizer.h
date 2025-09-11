// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ComponentVisualizer.h"

#define UEEDITOR_API ARRANGEREDITOR_API

/**
 * 
 */
class FARRangeDetectorComponentVisualizer final : public FComponentVisualizer
{

public:
  static UEEDITOR_API TSharedPtr<FComponentVisualizer> MakeInstance();

private:
  /**Start FComponentVisualizer Interface */
  UEEDITOR_API void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override final;
  /**End FComponentVisualizer Interface */
};

#undef UEEDITOR_API
