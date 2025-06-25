// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"

#include "MStateDefinition.generated.h"

class UMStateInstance;

USTRUCT(BlueprintType)
struct FMStateTransitionInfo
{
	GENERATED_BODY()

public:
	FString ToString() const;

	UPROPERTY(EditAnywhere, Category = "MState|Transition")
	FGameplayTag StateTag;

	UPROPERTY(EditAnywhere, Category = "MState|Transition")
	TSet<FGameplayTag> NextTransitionTags;

};

UCLASS(BlueprintType, Blueprintable, Const, Abstract)
class MSTATEMACHINE_API UMStateDefinition : public UObject
{
	GENERATED_BODY()

public:
	UMStateDefinition();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "MState|Definition")
	TSubclassOf<UMStateInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = "MState|Definition")
	FMStateTransitionInfo TransitionInfo;
};
