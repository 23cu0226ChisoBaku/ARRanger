//*************************************************
// キャラクターHPバーの基底クラス
//*************************************************

#include "Character/CharHPBar.h"

/**
 * @brief バトル状態かどうか通知を受け取る
 * 
 * @param true:バトル中　false:バトル中じゃない
 */
void UCharHPBar::BattleStateNotify(const bool onBattle)
{
    m_OnBattle = onBattle;
    DisplayBar(onBattle);
}

/**
 * @brief HPに変動があった際に変更前と変更後のHPを受け取る
 * 
 * @param maxHP:キャラクターの最大HP, prevoisHP:変更前のHP, postHP:変更後のHP
 */
void UCharHPBar::SetFluctuationHP(int32 maxHP, int32 prevoisHP, int32 postHP)
{
    /*現HPゲージの表示するパーセントを計算・通知*/
    float HPUnitQuantity = 1.0 / (float)maxHP;
    float HPGaugePercent = HPUnitQuantity * postHP;
    OnHPChangeEvent.Broadcast(HPGaugePercent);

    /*アニメーション*/
    
    /*バトル以外でHPが全回復している場合*/
    if(postHP == maxHP)
    {
        /*HPバーを非表示*/
        DisplayBar(false);
    }
}

/**
 * @brief HPバーの増減アニメーションを行う関数
 */
void UCharHPBar::StartAnimation()
{
    ;
}

/**
 * @brief HPバーを表示・非表示する
 * 
 * @param true:表示アニメーション　false:非表示アニメーション
 */
void UCharHPBar::DisplayBar(const bool display)
{
    /*表示・非表示*/
    const ESlateVisibility isDisplayType = display ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
    SetVisibility(isDisplayType);   
}