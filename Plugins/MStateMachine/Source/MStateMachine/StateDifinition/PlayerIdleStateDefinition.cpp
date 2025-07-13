// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerIdleStateDefinition.h"
#include "MStateMachine/StateInstance/PlayerIdleState.h"

UPlayerIdleStateDefinition::UPlayerIdleStateDefinition()
{
	// この定義に対応するインスタンスクラス
	InstanceType = UPlayerIdleState::StaticClass();

	// このステートの識別タグ
	TransitionInfo.StateTag = FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Idle"));

	// 遷移可能なステートタグ
	TransitionInfo.NextTransitionTags = {
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Walk")),
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Run")),
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Attack")),
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Jump"))
	};
}