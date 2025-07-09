// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Unreal Engine �� �y�ʂ� UObject �x�[�X�^���`���邽�߂̃w�b�_
// USTRUCT() �� UCLASS() ���g�����N���X�E�\���̂����Ƃ��Ɏg��
#include "UObject/NoExportTypes.h"

// FGameplayTag, FGameplayTagContainer �Ȃǂ� Gameplay Tag �V�X�e�����g�����߂̃w�b�_
// UE �̕W���I�ȃL�[���[�h�ɂ���ԁE�@�\�̎��ʃV�X�e��
#include "GameplayTagContainer.h"

// LogMState �� LogMStateMachine �Ƃ������O�J�e�S�����́i��`�j���s���Ă���
#include "MStateMachineLogChannels.h"

#include "MStateInstance.generated.h"

/**
 * �X�e�[�g�̖{��
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class MSTATEMACHINE_API UMStateInstance : public UObject
{
	GENERATED_BODY()

public:
	UMStateInstance(const FObjectInitializer& = FObjectInitializer::Get());
	virtual void BeginDestroy() override;

public:
	virtual void EntryState();
	virtual void TickState(float inDeltaTime);
	virtual void ExitState();

	UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "EntryState"))
	void K2_EntryState();

	UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "TickState"))
	void K2_TickState(float InDeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "MState|Instance", meta = (DisplayName = "ExitState"))
	void K2_ExitState();
};
