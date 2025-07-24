// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "CustomRPWorldSubsystem.generated.h"

namespace AR
{
namespace CustomRP
{
  class FMangaExaggerationStyleSVExtension;
}
}

/**
 * 
 */
UCLASS()
class UCustomRPWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

  using FMangeExaggerationStyleSVExtension = AR::CustomRP::FMangaExaggerationStyleSVExtension;

  public:
    /**Start USubsystem interface */
    ARRANGER_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    ARRANGER_API virtual void Deinitialize() override;
    /**End USubsystem interface */

  private:

    TSharedPtr<FMangeExaggerationStyleSVExtension, ESPMode::ThreadSafe> ExaggerationSceneViewExtension;
};
