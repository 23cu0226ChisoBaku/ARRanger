// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"

#include "MStateDefinition.generated.h"

/**前方宣言 */
class UMStateInstance;

/**
 * ステートタグ情報
 */
USTRUCT(BlueprintType)
struct FMStateTagInfo
{
	GENERATED_BODY()

public:
  /**
   * @brief 現在のステートタグを返す
   * 
   * @return StateTag文字列
   */
	FString ToString() const;

  /**
   * ステートタグ
   */
	UPROPERTY(EditAnywhere, Category = "MState|Transition")
	FGameplayTag StateTag;

  /**
   * 遷移できるステートのタグコンテナ
   */
	UPROPERTY(EditAnywhere, Category = "MState|Transition")
	TSet<FGameplayTag> NextTransitionTags;

};


/**
 * ステート定義オブジェクト
 */
UCLASS(BlueprintType, Blueprintable, Const, Abstract)
class MSTATEMACHINE_API UMStateDefinition : public UObject
{
	GENERATED_BODY()

public:
	UMStateDefinition();
	
public:

  /**
   * ステートインスタンスクラス
   */
	UPROPERTY(EditDefaultsOnly, Category = "MState|Definition")
	TSubclassOf<UMStateInstance> InstanceType;

  /**
   * ステートタグ情報
   */
	UPROPERTY(EditDefaultsOnly, Category = "MState|Definition")
	FMStateTagInfo TagInfo;
};
