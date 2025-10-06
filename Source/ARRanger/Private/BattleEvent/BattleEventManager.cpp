//*************************************************
// バトルイベントの管理クラス
//*************************************************

#include "Public/BattleEvent/BattleEventManager.h"
#include "Public/BattleEvent/BattleEventField.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ABattleEventManager::ABattleEventManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleEventManager::BeginPlay()
{
    Super::BeginPlay();

    /*レベル上の全てのフィールドのデリゲートをバインド*/
    for (ABattleEventField* field : m_BattleFields)
    {
        if (field == nullptr) 
        {
            continue;
        }

        /*各フィールド内のスポナー取得完了時の処理*/
        field->OnSpawnersCollected.AddDynamic(this, &ABattleEventManager::HandleFieldSpawnersCollected);
        
        field->OnBattleEventStart.AddDynamic(this, &ABattleEventManager::HandleBattleEventStart);
        field->OnBattleEventEnd.AddDynamic(this, &ABattleEventManager::HandleBattleEventEnd);    
    }
}

void ABattleEventManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (OnAnyFieldBattleEnd.IsBound())
    {
        OnAnyFieldBattleEnd.Broadcast();
    }

    Super::EndPlay(EndPlayReason);
}

/**
 * @brief バトルフィールドがスポナーを収集完了
 * 
 * @param 範囲内のスポナーの収集が終わったフィールド
 */
void ABattleEventManager::HandleFieldSpawnersCollected(ABattleEventField* field)
{
    if (field == nullptr)
    {
        return;
    }

    /*初期化済みリストに保持*/
    if (!m_InitializedFields.Contains(field))
    {
        m_InitializedFields.Add(field);
    }

    /*全フィールドの初期化が完了していれば、初回バトルフィールドをアクティブ化*/
    if (IsAllFieldsInitialized() && m_ActiveFields.Num() == 0)
    {
        BeginInitialBattleEvent();
    }
}


/**
 * @brief レベル上の全てのバトルフィールドが初期化済みかどうかを返す
 * 
 * @return レベル上の全てのバトルフィールドが初期化済みかどうか
 */
bool ABattleEventManager::IsAllFieldsInitialized() const
{
    return m_InitializedFields.Num() >= m_BattleFields.Num();
}

/**
 * @brief 一番最初とインデックス分のフィールドをアクティブ化
 */
void ABattleEventManager::BeginInitialBattleEvent()
{
    if (m_BattleFields.Num() > 0)
    {
        SetActiveField(m_BattleFields[0]);
        if(m_ActiveFieldOffset != 0)
        {
            ActivateNextFields();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleEventManager does not have a BattleEventFiled set."));
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

    // バリア出現時のSEを再生
    UGameplayStatics::PlaySound2D(
        GetWorld(),
        SE_Barrier_Start,
        3.0f,
        1.0f
    );
}

/**
 * @brief バトルイベント終了時の処理 
 * 
 * @param バトルイベントが終了したフィールド
 */ 
void ABattleEventManager::HandleBattleEventEnd(ABattleEventField* finishedBattleField)
{
    /*外部クラス用*/
    OnAnyFieldBattleEnd.Broadcast();
    /*次のフィールドをアクティブ化*/
    ActivateNextFields();

    // バリア消失時のSEを再生
    UGameplayStatics::PlaySound2D(
        GetWorld(),
        SE_Barrier_End,
        2.0f,
        1.0f
    );
}

/** 
 * @brief 次のフィールドをオフセット分稼働させる 
 */
void ABattleEventManager::ActivateNextFields()
{
    if (m_ActiveFields.Num() < 0)
    {
        return;
    }

    /*最後にアクティブ化したフィールドを基準にする*/
    ABattleEventField* currentField = m_ActiveFields.Last();
    if (currentField == nullptr || !m_BattleFields.Contains(currentField))
    {
        return;
    }

    int32 currentIndex = m_BattleFields.IndexOfByKey(currentField);
    if (currentIndex == INDEX_NONE)
    {
        return;
    }

    /*もしオフセットがない場合*/
    if(m_ActiveFieldOffset == 0)
    {
        SetActiveField(m_BattleFields[currentIndex + 1]);
    }
    else
    {
        /*オフセット分だけ次のフィールドを順に稼働させる*/
        for (int32 offset = 1; offset <= m_ActiveFieldOffset; ++offset)
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
}

/**
 * @brief 指定のフィールドを稼働させる
 * 
 * @param 稼働させるフィールド
 */
void ABattleEventManager::SetActiveField(ABattleEventField* newField)
{
    if (newField == nullptr)
    {
        return;
    }

    /*まだアクティブでないなら処理*/
    if (newField->IsActivedField() || m_ActiveFields.Contains(newField))
    {
        return;
    }
    
    /*フィールドをアクティブ化し保持する*/
    newField->ActiveEventField();
    m_ActiveFields.Add(newField);
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