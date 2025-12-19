//*************************************************
// キャラクターHPバーの基底クラス
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharHPBar.generated.h"

UCLASS()
class UCharHPBar : public UUserWidget
{
	GENERATED_BODY()

public:

	/**
	 * @brief キャラクターの最大HPを保持
	 * 
	 * @param キャラクターの最大HP
	 */
	void SetHPBar(const int32 MaxHP);

	/**
	 * @brief バトル状態かどうか通知を受け取る
	 * 
	 * @param true:バトル中　false:バトル中じゃない
	 */
	void BattleStateNotify(const bool OnBattle);

	/**
	 * @brief HPに変動があった際に変更前と変更後のHPを受け取る
	 * 
	 * @param true:バトル中　false:バトル中じゃない
	 */
	void SetHPNum(int32 prevoisHP, int32 postHP);

private:

	/**
	 * @brief HPバーの増減アニメーションを行う関数
	 */
	void StartAnimation();

	/**
	 * @brief HPバーを表示・非表示する
	 * 
	 * @param true:表示アニメーション　false:非表示アニメーション
	 */
	void DisplayBar(const bool display);

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HPBarAnimation;	/*HPバーを増量・減量させるアニメーション*/

	int32 m_MaxHP;		/*最大のHP*/
	int32 m_CurrentHP;	/*現在のHP*/
	int32 m_PrevoisHP;	/*変更前のHP*/
	int32 m_PostHP;		/*変更後のHP*/
	bool m_OnBattle;	/*バトル状態かどうか*/
};
