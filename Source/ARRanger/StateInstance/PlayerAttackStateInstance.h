//*************************************************
// ƒvƒŒƒCƒ„[‚ÌUŒ‚ó‘Ô‚Ìˆ—(‹““®)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerAttackStateInstance.generated.h"

/**
 * Player Attack Instance
 */
UCLASS()
class ARRANGER_API UPlayerAttackStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;

};
