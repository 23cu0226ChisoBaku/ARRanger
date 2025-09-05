// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MStateContextFunctions.generated.h"

/**前方宣言 */
struct FStateTransitionParameters;
struct FStateTickParameters;

#define UE_API MSTATEMACHINE_API

UCLASS()
class UMStateContextFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
  public:
    UFUNCTION(BlueprintCallable, Category="State Transition|Context", meta=(WorldContext="WorldContextObject", DisplayName="Get Owner In State Transition"))
    static MSTATEMACHINE_API UObject* GetOwnerByTransitionParams(const UObject* WorldContextObject, const FStateTransitionParameters& InParameters);

    UFUNCTION(BlueprintCallable, Category="State Transition|Context", meta=(WorldContext="WorldContextObject",DisplayName="Get Owner Controller In State Transition"))
    static MSTATEMACHINE_API AController* GetOwnerControllerByTransitionParams(const UObject* WorldContextObject, const FStateTransitionParameters& InParameters);

    UFUNCTION(BlueprintCallable, Category="State Tick|Context", meta=(WorldContext="WorldContextObject",DisplayName="Get Owner In State Tick"))
    static MSTATEMACHINE_API UObject* GetOwnerByTickParams(const UObject* WorldContextObject, const FStateTickParameters& InParameters);

    UFUNCTION(BlueprintCallable, Category="State Tick|Context", meta=(WorldContext="WorldContextObject",DisplayName="Get Owner Controller In State Tick"))
    static MSTATEMACHINE_API AController* GetOwnerControllerByTickParams(const UObject* WorldContextObject, const FStateTickParameters& InParameters);
};

#undef UE_API
