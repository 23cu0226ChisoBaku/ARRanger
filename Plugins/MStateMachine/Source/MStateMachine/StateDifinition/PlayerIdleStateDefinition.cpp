// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerIdleStateDefinition.h"
#include "MStateMachine/StateInstance/PlayerIdleState.h"

UPlayerIdleStateDefinition::UPlayerIdleStateDefinition()
{
	// ���̒�`�ɑΉ�����C���X�^���X�N���X
	InstanceType = UPlayerIdleState::StaticClass();

	// ���̃X�e�[�g�̎��ʃ^�O
	TransitionInfo.StateTag = FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Idle"));

	// �J�ډ\�ȃX�e�[�g�^�O
	TransitionInfo.NextTransitionTags = {
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Walk")),
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Run")),
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Attack")),
		FGameplayTag::RequestGameplayTag(TEXT("PlayerState_Jump"))
	};
}