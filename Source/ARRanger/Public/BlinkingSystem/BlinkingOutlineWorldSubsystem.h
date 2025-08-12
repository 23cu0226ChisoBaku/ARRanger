//*************************************************
// アウトラインの点滅処理を制御するワールドシステム
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Public/BlinkingSystem/BlinkingOutlineSystem.h"
#include "BlinkingOutlineWorldSubsystem.generated.h"

// 前方宣言
class ULineTraceSingleARObjectComponent;
class ABlinkOutlineTickActor;


/*
 * BlinkingOutlineSystem の関数を LineTraceSingleARObjectComponent のデリゲートにバインドする用のシステム
 */
UCLASS()
class ARRANGER_API UBlinkingOutlineWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/*
	* @brief BlinkingOutlineSystem を初期化し、ライントレースコンポーネントにデリゲートをバインド
	*
	* @param 現在のワールド, ライントレースコンポーネント, 点滅処理させるアクターのクラス
	*/
	void SetupBlinkingSystem(UWorld* World, ULineTraceSingleARObjectComponent* lineTraceComp, TSubclassOf<ABlinkOutlineTickActor> tickActorClass);

	FBlinkingOutlineSystem* GetBlinkingOutlineSystem() const { return m_BlinkingOutlineSystem.Get(); }

private:

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
	UPROPERTY()
	TSubclassOf<ABlinkOutlineTickActor> m_TickActorClass;

	TUniquePtr<class FBlinkingOutlineSystem> m_BlinkingOutlineSystem;
};