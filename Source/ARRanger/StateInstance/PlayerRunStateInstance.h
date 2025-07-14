//*************************************************
// プレイヤーの走り状態の処理(挙動)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerRunStateInstance.generated.h"

/**
 * プレイヤーの走り状態の処理(挙動)
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
