//*************************************************
// ƒvƒŒƒCƒ„[‚Ì—‰ºó‘Ô‚Ìˆ—(‹““®)
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "MStateInstance.h"
#include "PlayerFallStateInstance.generated.h"

/**
 * Player Fall Instance
 */
UCLASS()
class ARRANGER_API UPlayerFallStateInstance : public UMStateInstance
{
	GENERATED_BODY()
	
public:
	void EntryState() override;
	void TickState(float inDeltaTime) override;
	void ExitState() override;

};
