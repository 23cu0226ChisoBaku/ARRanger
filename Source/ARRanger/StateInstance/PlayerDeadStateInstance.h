//*************************************************
// �v���C���[�̎��S��Ԃ̏���(����)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerDeadStateInstance.generated.h"

/**
 * Player Dead Instance
 */
UCLASS()
class ARRANGER_API UPlayerDeadStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;

};
