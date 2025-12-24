//*************************************************
// キャラクターHPバーの基底クラス
//*************************************************

#include "Character/HPBar.h"

/**
 * @brief バトル状態かどうか通知を受け取る
 * 
 * @param true:バトル中　false:バトル中じゃない
 */
void UHPBar::BattleStateNotify(const bool onBattle)
{
    m_OnBattle = onBattle;

    /*バトルに突入したらHP表示*/
    if(m_OnBattle)
    {
        DisplayBar(m_OnBattle);
    }
}

/**
 * @brief HPに変動があった際に変更前と変更後のHPを受け取る
 * 
 * @param maxHP:キャラクターの最大HP, prevoisHP:変更前のHP, postHP:変更後のHP
 */
void UHPBar::SetFluctuationHP(int32 maxHP, int32 prevoisHP, int32 postHP)
{
    /*現HPゲージの表示するパーセントを計算・通知*/
    float HPUnitQuantity = 1.0 / (float)maxHP;
    float HPGaugePercent = HPUnitQuantity * postHP;
    OnHPChangeEvent.Broadcast(HPGaugePercent);

    /*アニメーション*/
    StartAnimation();
    
    /*バトル以外でHPが全回復している場合*/
    if(!m_OnBattle && postHP == maxHP)
    {
        /*HPバーを非表示*/
        DisplayBar(false);
    }
}

/**
 * @brief HPバーの増減アニメーションを行う関数
 */
void UHPBar::StartAnimation()
{
    ;
}

/**
 * @brief HPバーを表示・非表示する
 * 
 * @param true:表示アニメーション　false:非表示アニメーション
 */
void UHPBar::DisplayBar(const bool display)
{
    /*表示・非表示*/
    const ESlateVisibility isDisplayType = display ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
    SetVisibility(isDisplayType);   
}