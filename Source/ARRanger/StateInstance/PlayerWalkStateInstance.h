//*************************************************
// プレイヤーの歩き状態の処理(挙動)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerWalkStateInstance.generated.h"

/**
 * プレイヤーの歩き状態
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
