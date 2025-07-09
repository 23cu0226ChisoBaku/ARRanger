// 現在のステートはどのタグを持っていて、どの状態に遷移できて、どんな動作（インスタンス）をするかをまとめたクラス

#include "MStateDefinition.h"

#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateDefinition)

// FMStateTransitionInfoが持つStateTagのステートを文字列で返す関数
FString FMStateTransitionInfo::ToString() const
{
	return StateTag.IsValid()? StateTag.ToString() : FGameplayTag::EmptyTag.ToString();
}

// コンストラクタ
UMStateDefinition::UMStateDefinition()
{
	// 状態に合わせた挙動を指定している。
	InstanceType = UMStateInstance::StaticClass();
}