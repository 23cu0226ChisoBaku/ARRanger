	// 現在のステートはどのタグを持っていて、どの状態に遷移できて、どんな動作（インスタンス）をするかをまとめたクラス

	#pragma once

	// UObject を継承するが、DLL（モジュール）間でエクスポートしないクラス・構造体を定義するための基本ヘッダー
	#include "UObject/NoExportTypes.h"
	#include "GameplayTagContainer.h"

	#include "MStateDefinition.generated.h"

	class UMStateInstance;

	// 構造体が Blueprint 内でも「変数」として使える
	// ステートの遷移情報をまとめるためのクラス
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

	// Blueprint の変数として使える
	// Blueprintベースクラスとして継承可能
	// Blueprint でこのクラスのオブジェクトが 既定で Const（変更不可） 扱い
	// インスタンス化できない抽象クラス
	// 現在の状態はどんなインスタンスで、どこに遷移できるかなどをまとめた状態定義クラス
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
