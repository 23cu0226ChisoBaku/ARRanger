//*************************************************
// �v���C���[�̑����Ԃ̏���(����)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerRunStateInstance.generated.h"

/**
 * �v���C���[�̑����Ԃ̏���(����)
 */
UCLASS()
class ARRANGER_API UPlayerRunStateInstance : public UMStateInstance
{
	GENERATED_BODY()

public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;
};
