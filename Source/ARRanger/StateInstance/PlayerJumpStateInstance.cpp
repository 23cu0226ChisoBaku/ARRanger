//*************************************************
// �v���C���[�̃W�����v��Ԃ̏���(����)
//*************************************************

#include "PlayerJumpStateInstance.h"

void UPlayerJumpStateInstance::EntryState()
{
	;
}

void UPlayerJumpStateInstance::TickState(float inDeltaTime)
{
	;
}

void UPlayerJumpStateInstance::ExitState()
{
	;
}

/*
* 
*/
void UPlayerJumpStateInstance::RequestJump(UObject* Target)
{
	if (!Target) return;

	if (Target->GetClass()->ImplementsInterface(UJumpInterface::StaticClass()))
	{
		IJumpInterface::Execute_IJump(Target);
	}
}
