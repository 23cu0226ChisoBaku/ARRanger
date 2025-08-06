// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ARPhysicsEngineTestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARRANGER_API AARPhysicsEngineTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
  virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

  UPROPERTY(EditDefaultsOnly)
  TSubclassOf<class AARPhysicsTickProcessorActor> TickProcessorClass;
};
