//*************************************************
// プレイヤーの待機状態の処理(挙動)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerIdleStateInstance.generated.h"

/**
 * Player Idle Instance
 */
UCLASS()
class ARRANGER_API UPlayerIdleStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;
};