//*************************************************
// �v���C���[�̑ҋ@��Ԃ̏���(����)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerJumpStateInstance.generated.h"

/**
 * Player Jump Instance
 */
UCLASS()
class ARRANGER_API UPlayerJumpStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;
};
