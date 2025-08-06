//*************************************************
// 引力斥力の付与に関する制御を行うゲームモード
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Test_BlinkingBindGameMode.generated.h"

// 前方宣言
class ULineTraceSingleARObjectComponent;
class BlinkingOutlineSystem;

/*
 * BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインドする用のゲームモード
 */
UCLASS()
class ARRANGER_API ATest_BlinkingBindGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	void StartPlay() override;

	/*
	 * @brief BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインド 
	 */
	void BindSetTargetMagnetizableObject();

private:
    ULineTraceSingleARObjectComponent* m_LineTraceComponent;
    BlinkingOutlineSystem* m_BlinkingOutlineSystem;
};
