// ���݂̃X�e�[�g�͂ǂ̃^�O�������Ă��āA�ǂ̏�ԂɑJ�ڂł��āA�ǂ�ȓ���i�C���X�^���X�j�����邩���܂Ƃ߂��N���X

#include "MStateDefinition.h"

#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateDefinition)

// FMStateTransitionInfo������StateTag�̃X�e�[�g�𕶎���ŕԂ��֐�
FString FMStateTransitionInfo::ToString() const
{
	return StateTag.IsValid()? StateTag.ToString() : FGameplayTag::EmptyTag.ToString();
}

// �R���X�g���N�^
UMStateDefinition::UMStateDefinition()
{
	// ��Ԃɍ��킹���������w�肵�Ă���B
	InstanceType = UMStateInstance::StaticClass();
}