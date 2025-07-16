// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRenderPipeline/CustomRPWorldSubsystem.h"

#include "CustomRenderPipeline/MangaExaggerationStyleSVExtension.h"

void UCustomRPWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);

  ExaggerationSceneViewExtension = FSceneViewExtensions::NewExtension<FMangeExaggerationStyleSVExtension>(this);

}

void UCustomRPWorldSubsystem::Deinitialize()
{
  Super::Deinitialize();

  ExaggerationSceneViewExtension->Invalidate();
}
