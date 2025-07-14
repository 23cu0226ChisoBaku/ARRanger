//*************************************************
// �v���C���[�̒��n�@��Ԃ̏���(����)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerLandStateInstance.generated.h"

/**
 * Player Land Instance
 */
UCLASS()
class ARRANGER_API UPlayerLandStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;

};
