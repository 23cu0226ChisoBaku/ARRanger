// ���W���[���̊J�n�E�I�����ɍs������������N���X

#include "MStateMachine.h"

#define LOCTEXT_NAMESPACE "FMStateMachineModule"

void FMStateMachineModule::StartupModule()
{
	// ���̃R�[�h�́A���W���[�����������Ƀ��[�h���ꂽ��Ɏ��s����܂��B���m�ȃ^�C�~���O�́A���W���[�����Ƃ�.uplugin�t�@�C���Ŏw�肵�܂��B
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMStateMachineModule::ShutdownModule()
{
	// ���̊֐��́A���W���[�����N���[���A�b�v���邽�߂ɃV���b�g�_�E�����ɌĂ΂�邱�Ƃ�����B ���I�����[�h���T�|�[�g���郂�W���[���̏ꍇ�A
	// ���W���[�����A�����[�h����O�ɂ��̊֐����Ăяo���B
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMStateMachineModule, MStateMachine)