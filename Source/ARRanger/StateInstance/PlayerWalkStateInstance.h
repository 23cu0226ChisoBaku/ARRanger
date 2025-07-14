//*************************************************
// �v���C���[�̕�����Ԃ̏���(����)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerWalkStateInstance.generated.h"

/**
 * �v���C���[�̕������
 */
UCLASS()
class ARRANGER_API UPlayerWalkStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;
};
