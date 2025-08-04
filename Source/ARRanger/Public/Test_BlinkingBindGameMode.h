
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BlinkingOutlineSystem.h"
#include "InstantScripts/LineTraceSingleARObjectComponent.h"
#include "Test_BlinkingBindGameMode.generated.h"

/*
 * BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインドする用のゲームモード
 */
UCLASS()
class ARRANGER_API ATest_BlinkingBindGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	void StartPlay() override;
};
