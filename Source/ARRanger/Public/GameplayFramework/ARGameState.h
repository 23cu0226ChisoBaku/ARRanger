// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"

#include "ARGameState.generated.h"

/**
 * 
 */
UCLASS()
class ARRANGER_API AARGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
  UPROPERTY(EditDefaultsOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
  TSubclassOf<AActor> StateMachineActorClass;
};
