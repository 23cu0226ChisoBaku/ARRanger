/**
 * @file ARInputMappingContext.h
 * @author MAI ZHICONG
 * @brief ARRangerインプットマッピングコンテキスト（Tag付き）
 */
#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARInputMappingContext.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class UInputMappingContext;

/**
 * @brief InputMappingContext with signature tag
 */
USTRUCT(BlueprintType)
struct FARInputMappingContextAndTag
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly)
  TSoftObjectPtr<UInputMappingContext> InputMapping;

  UPROPERTY(EditDefaultsOnly)
  FGameplayTag InputStateTag;

};

UCLASS(Const)
class UARInputMappingContext : public UDataAsset
{
	GENERATED_BODY()

public:

  /**
   * @brief 与えられたTagでIMCを探す
   * @param InTag IMCにバインドしたTag
   * @see InputMappingContexts
   */
  UE_API UInputMappingContext* FindIMCWithTag(const FGameplayTag& InTag) const;
  
  UPROPERTY(EditDefaultsOnly, Category = "InputMappingContext", meta = (TitleProperty = "{InputMapping} with state tag: {InputStateTag}"))
  TArray<FARInputMappingContextAndTag> InputMappingContexts;
};

#undef UE_API
