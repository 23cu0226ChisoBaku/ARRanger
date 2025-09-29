/**
 *  ARInputMappingContext.h
 *  InputMappingContext for ARRanger project use
 */

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARInputMappingContext.generated.h"

#define UE_API ARRANGER_API

/**Forward declaration */
class UInputMappingContext;

/**
 * @brief InputMappingContext with signature tag
 */
USTRUCT(BlueprintType)
struct FARInputMappingContextAndTag
{
  GENERATED_BODY()

  /**InputMappingContext */
  UPROPERTY(EditDefaultsOnly)
  TSoftObjectPtr<UInputMappingContext> InputMapping;

  /**Signature tag */
  UPROPERTY(EditDefaultsOnly)
  FGameplayTag InputStateTag;

};

/**
 * @brief InputMappingContext for ARRanger project use
 */
UCLASS(Const)
class UARInputMappingContext : public UDataAsset
{
	GENERATED_BODY()

public:

  /**
   * @brief Find InputMappingContext by signature tag
   * @param InTag Signature tag
   * 
   * @return UInputMappingContext Found UE native InputMappingContext
   */
  UE_API UInputMappingContext* FindIMCWithTag(const FGameplayTag& InTag) const;
  
  /**List of InputMappingContext with signature tag */
  UPROPERTY(EditDefaultsOnly, Category = "InputMappingContext", meta = (TitleProperty = "{InputMapping} with state tag: {InputStateTag}"))
  TArray<FARInputMappingContextAndTag> InputMappingContexts;
};

#undef UE_API
