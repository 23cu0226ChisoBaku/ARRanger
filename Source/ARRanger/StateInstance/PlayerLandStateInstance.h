//*************************************************
// プレイヤーの着地機状態の処理(挙動)
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
