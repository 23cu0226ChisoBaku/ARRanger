// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

#include "ARRepositoryAutoRegisterSystem.generated.h"

/**
 * 
 */
UCLASS()
class UARRepositoryAutoRegisterSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

  ARRANGER_API virtual void Initialize(FSubsystemCollectionBase& Collection);
	ARRANGER_API virtual void Deinitialize();

#if WITH_EDITORONLY_DATA

  public:
    TMulticastDelegate<void()> OnRepositoryRegistered;
    TMulticastDelegate<void()> OnRepositoryUnregistered;

#endif
};
