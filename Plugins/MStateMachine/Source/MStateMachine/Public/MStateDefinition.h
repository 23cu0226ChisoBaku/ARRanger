	// ���݂̃X�e�[�g�͂ǂ̃^�O�������Ă��āA�ǂ̏�ԂɑJ�ڂł��āA�ǂ�ȓ���i�C���X�^���X�j�����邩���܂Ƃ߂��N���X

	#pragma once

	// UObject ���p�����邪�ADLL�i���W���[���j�ԂŃG�N�X�|�[�g���Ȃ��N���X�E�\���̂��`���邽�߂̊�{�w�b�_�[
	#include "UObject/NoExportTypes.h"
	#include "GameplayTagContainer.h"

	#include "MStateDefinition.generated.h"

	class UMStateInstance;

	// �\���̂� Blueprint ���ł��u�ϐ��v�Ƃ��Ďg����
	// �X�e�[�g�̑J�ڏ����܂Ƃ߂邽�߂̃N���X
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

	// Blueprint �̕ϐ��Ƃ��Ďg����
	// Blueprint�x�[�X�N���X�Ƃ��Čp���\
	// Blueprint �ł��̃N���X�̃I�u�W�F�N�g�� ����� Const�i�ύX�s�j ����
	// �C���X�^���X���ł��Ȃ����ۃN���X
	// ���݂̏�Ԃ͂ǂ�ȃC���X�^���X�ŁA�ǂ��ɑJ�ڂł��邩�Ȃǂ��܂Ƃ߂���Ԓ�`�N���X
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
