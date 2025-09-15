//*************************************************
// バトルイベントのフェーズ
//*************************************************

#pragma once
#include "CoreMinimal.h"
#include "SpawnPhase.generated.h"

/** スポーンフェーズの種類 */
UENUM(BlueprintType)
enum class ESpawnPhase : uint8
{
    Phase1 UMETA(DisplayName="Phase 1"),
    Phase2 UMETA(DisplayName="Phase 2"),
    Phase3 UMETA(DisplayName="Phase 3")
};