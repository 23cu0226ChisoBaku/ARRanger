//*************************************************
// アウトラインの点滅処理を制御するワールドシステム
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Public/BlinkingSystem/OutlineSystem.h"
#include "BlinkingOutlineWorldSubsystem.generated.h"

// 前方宣言
class UDetectorMagnetizableComponent;
class AOutlineTickActor;

/*
 * OutlineSystem の関数を DetectorMagnetizableComponent のデリゲートにバインドする用のシステム
 */
UCLASS()
class ARRANGER_API UBlinkingOutlineWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/*
	* @brief OutlineSystem を初期化し、ライントレースコンポーネントにデリゲートをバインド
	*
	* @param 現在のワールド, ライントレースコンポーネント, 点滅処理させるアクターのクラス
	*/
	void SetupBlinkingSystem(UWorld* World, UDetectorMagnetizableComponent* lineTraceComp, TSubclassOf<AOutlineTickActor> tickActorClass);

	FOutlineSystem* GetBlinkingOutlineSystem() const { return m_OutlineSystem.Get(); }

private:

	/*
	 * @brief OutlineSystem の関数を DetectorMagnetizableComponent のデリゲートにバインド 
	 */
	void BindBlinkingMagnetizableObjectDelegate();

	/*
	 * @brief バインドされているデリゲート関数をアンバインドする 
	 */
	void UnBindDelegate();

private:

	UPROPERTY()
	TObjectPtr<UDetectorMagnetizableComponent> m_DetectorMagnetizableComponent;
	UPROPERTY()
	TSubclassOf<AOutlineTickActor> m_TickActorClass;

	TUniquePtr<class FOutlineSystem> m_OutlineSystem;
};