//*************************************************
// 引力斥力の付与に関する制御を行うゲームモード
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "Test_BlinkingBindGameMode.generated.h"

// 前方宣言
class ULineTraceSingleARObjectComponent;
class FBlinkingOutlineSystem;
class ABlinkOutlineTickActor;

/*
 * BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインドする用のゲームモード
 */
UCLASS()
class ARRANGER_API ATest_BlinkingBindGameMode : public AGameModeBase
{
	GENERATED_BODY()

	public:
		ATest_BlinkingBindGameMode();
	
	private:
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*
	 * @brief BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインド 
	 */
	void BindBlinkingMagnetizableObjectDelegate();

	/*
	 * @brief バインドされているデリゲート関数をアンバインドする 
	 */
	void UnBindDelegate();

private:

	UPROPERTY()
    TObjectPtr<ULineTraceSingleARObjectComponent> m_LineTraceComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABlinkOutlineTickActor> TickActorClass = nullptr;

	TUniquePtr<FBlinkingOutlineSystem> m_BlinkingOutlineSystem;
};
