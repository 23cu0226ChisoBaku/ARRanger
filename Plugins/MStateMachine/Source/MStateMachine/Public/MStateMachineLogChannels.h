// ���e�F ���̃R�[�h�t�@�C������uLogMState�v�uLogMStateMachine�v�Ƃ������O�̃��O�J�e�S�����g����悤�ɐ錾

// �C���N���[�h�K�[�h
#pragma once

// UE�̃w�b�_�[�t�@�C��
// UE�Ɋւ��郍�O�̋@�\��ێ����Ă���
#include "Logging/LogMacros.h"

// LogMState �� LogMStateMachine �Ƃ������O�J�e�S���́u�O���錾�v�����Ă���B

// DECLARE_LOG_CATEGORY_EXTERN �́u�O������g�����߂̐錾�v�ł���A
// ���ۂ̒�`�͕ʂ�cpp�t�@�C���� DEFINE_LOG_CATEGORY(LogMState); �̂悤�ɏ�����Ă���B
// MSTATEMACHINE_API �̓��W���[���̊O���A�N�Z�X��������G�N�X�|�[�g / �C���|�[�g�}�N���B
// ����ɂ�葼�̃��W���[������������̃��O�J�e�S�����Q�Ƃł���B

MSTATEMACHINE_API DECLARE_LOG_CATEGORY_EXTERN(LogMState, Log, All);
MSTATEMACHINE_API DECLARE_LOG_CATEGORY_EXTERN(LogMStateMachine, Log, All);