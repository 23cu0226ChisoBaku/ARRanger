// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TestUInterface.generated.h"

UENUM(BlueprintType)
enum struct ETestTargetClassType : uint8
{
  Actor,
  Interface,
};


/**
 * 
 */
UCLASS(BlueprintType)
class ARRANGER_API UTestUInterface : public UDataAsset
{
	GENERATED_BODY()
	

public:

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  ETestTargetClassType Type;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetClass", meta = (EditCondition = "Type == ETestTargetClassType::Actor", EditConditionHides))
  TSubclassOf<AActor> TargetActor;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TargetClass", meta = (EditCondition = "Type == ETestTargetClassType::Interface", EditConditionHides))
  TSubclassOf<UInterface> TargetInterface;
};
