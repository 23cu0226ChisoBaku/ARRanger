// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRenderPipeline/CustomRPWorldSubsystem.h"

#include "CustomRenderPipeline/MangaExaggerationStyleSVExtension.h"

#include "Kismet/GameplayStatics.h"

void UCustomRPWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);

  ExaggerationSceneViewExtension = FSceneViewExtensions::NewExtension<FMangeExaggerationStyleSVExtension>(this);

  GetWorld()->GetWorldSettings()->SetTimeDilation(0.2f);
}

void UCustomRPWorldSubsystem::Deinitialize()
{
  Super::Deinitialize();

  ExaggerationSceneViewExtension->Invalidate();

  GetWorld()->GetWorldSettings()->SetTimeDilation(1.f);
}
