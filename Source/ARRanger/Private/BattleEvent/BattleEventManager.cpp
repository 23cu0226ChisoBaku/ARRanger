//*************************************************
// バトルイベントの管理クラス CPP
//*************************************************

#include "BattleEventManager.h"
#include "BattleEventField.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

ABattleEventManager::ABattleEventManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleEventManager::BeginPlay()
{
    Super::BeginPlay();

    /*レベル上の全てのフィールドのデリゲートにバインド*/
    for (ABattleEventField* field : m_BattleFields)
    {
        if (field != nullptr)
        {
            field->OnBattleEventStart.AddDynamic(this, &ABattleEventManager::HandleBattleEventStart);
            field->OnBattleEventEnd.AddDynamic(this, &ABattleEventManager::HandleBattleEventEnd);
        }
    }

    /*一番最初とインデックス分のフィールドをアクティブ化*/
    if (m_BattleFields.Num() > 0)
    {
        SetActiveField(m_BattleFields[0]);
        ActivateNextFields();
    }
}

/** 
 * @brief バトルイベント開始時の処理 
 * 
 * @param バトルイベントが開始したフィールド 
 */
void ABattleEventManager::HandleBattleEventStart(ABattleEventField* startedBattleField)
{
    /*現在進行中のイベントフィールドを保持*/
    m_CurrentBattleField = startedBattleField;

    /*外部クラス用*/
    OnAnyFieldBattleStart.Broadcast();
}

/**
 * @brief バトルイベント終了時の処理 
 * 
 * @param バトルイベントが終了したフィールド
 */ 
void ABattleEventManager::HandleBattleEventEnd(ABattleEventField* finishedBattleField)
{
    /*終了したイベントフィールドを破棄*/
    if (m_CurrentBattleField == finishedBattleField)
    {
        m_CurrentBattleField = nullptr;
    }

    /*外部クラス用*/
    OnAnyFieldBattleEnd.Broadcast();

    /*次のフィールドをアクティブ化*/
    ActivateNextFields();
}

/** 
 * @brief 次のフィールドをオフセット分稼働させる 
 */
void ABattleEventManager::ActivateNextFields()
{
    if (m_ActiveField == nullptr || !m_BattleFields.Contains(m_ActiveField)) 
    {
        return;
    }

    int32 currentIndex = m_BattleFields.IndexOfByKey(m_ActiveField);
    if (currentIndex == INDEX_NONE)
    {
        return;
    }

    /*オフセット分だけ次のフィールドを順に稼働させる*/
    for (int32 offset = 1; offset <= nextFieldOffset; ++offset)
    {
        int32 nextIndex = currentIndex + offset;
        if (m_BattleFields.IsValidIndex(nextIndex))
        {
            ABattleEventField* nextField = m_BattleFields[nextIndex];
            if (nextField != nullptr && !nextField->IsActivedField())
            {
                SetActiveField(nextField);
            }
        }
    }
}

/**
 * @brief 指定のフィールドを稼働させる
 * 
 * @param 稼働させるフィールド
 */
void ABattleEventManager::SetActiveField(ABattleEventField* newField)
{
    if (newField != nullptr)
    {
        m_ActiveField->ActiveEventField();
        m_ActiveField = newField;
    }
}

/** 
 * @brief 進行中のフィールドの残りの敵数を取得 
 * 
 * @return 進行中のフィールドの残りの敵数 
 */ 
int32 ABattleEventManager::GetCurrentRemainingEnemiesInField() const
{
    return m_CurrentBattleField ? m_CurrentBattleField->GetRemainingEnemiesInField() : 0;
}

/** 
 * @brief 進行中のフェーズの残りの敵数を取得
 * 
 * @return 進行中のフェーズの残りの敵数 
 */ 
int32 ABattleEventManager::GetCurrentRemainingEnemiesInPhase() const
{
    return m_CurrentBattleField ? m_CurrentBattleField->GetRemainingEnemiesInPhase() : 0;
}