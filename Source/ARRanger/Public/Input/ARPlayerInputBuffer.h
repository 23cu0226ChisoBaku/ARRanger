// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"

#include "GameplayTagContainer.h"

#include "ARPlayerInputBuffer.generated.h"

#define UE_API ARRANGER_API

class AARRangerPlayerController;
class UARInputComponent;
class UARInputConfig;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UARPlayerInputBuffer : public UObject
{
	GENERATED_BODY()

public:

  UE_API UARPlayerInputBuffer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  UE_API void InitializeInputBuffer(UARInputComponent* InInputComponent, const UARInputConfig& InInputConfig);

  UE_API void UninitializeInputBuffer();

  UE_API void EvaluateBuffer(const AARRangerPlayerController* InPlayerController, const float DeltaTime, const bool bGamePaused);

  UE_API bool IsInputBufferValid(const FGameplayTag& InInputTag) const;

  UE_API void ClearAllInputs();

  UE_API void HandleInputTagPressed(const FGameplayTag& InInputTag);
  
  UE_API void HandleInputTagReleased(const FGameplayTag& InInputTag);

private:

  void ConsumeBuffer(const FGameplayTag& InInputTag);

private:

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  float InputKeepTime = 0.0f;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Input", meta = (AllowPrivateAccess = "true"))
  bool bEvaluateIfGamePaused = false;

  UPROPERTY()
  TObjectPtr<UARInputComponent> InputComponent;

  TMap<FGameplayTag, float> m_inputTagBuffers;

  TArray<FGameplayTag> m_removeTags;

  TArray<uint32> m_bindHandles;
	
};

#undef UE_API