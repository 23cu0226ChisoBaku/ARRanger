//*************************************************
// バトルイベントのフィールドクラス
//*************************************************

#include "public/BattleEvent/BattleEventField.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Public/BattleEvent/EnemySpawner.h"
#include "Public/BattleEvent/BattleEventCage.h"

ABattleEventField::ABattleEventField()
    : m_Player(nullptr)               
    , m_RemainingEnemiesInField(0) 
    , m_RemainingEnemiesInPhase(0)    
    , m_CurrentPhaseIndex(0)
    , m_IsActiveField(false)
    , m_EventTriggered(false)

{
    PrimaryActorTick.bCanEverTick = true;
}

void ABattleEventField::BeginPlay()
{
    Super::BeginPlay();

    /*Tick処理を無効化*/
    SetActorTickEnabled(false);

    /*バトルフィールド範囲用（コリジョン）コンポーネントを全て取得*/
    GetPrimitiveComponents();

    /*各範囲コリンジョンコンポーネントににオーバーラップ関数をバインド*/
    for (UPrimitiveComponent* primComp : m_PrimitiveComponents)
    {
        if (primComp == nullptr) 
        {
            continue;
        }
        primComp->OnComponentBeginOverlap.AddDynamic(this, &ABattleEventField::OnFieldBeginOverlap);
    }

    /*バリアエフェクトのコンポーネントを取得する*/ 
    if(m_BarrierEffectActor != nullptr)
    {
        UNiagaraComponent* niagaraComp = m_BarrierEffectActor->GetNiagaraComponent();
        if (niagaraComp)
        {
            m_BarrierEffectComp = niagaraComp;
            /*最初はナイアガラを再生しない*/
            m_BarrierEffectComp->bAutoActivate = false;
        }
    }
    
    /*次フレームで範囲内のスポナーを取得*/
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABattleEventField::CollectSpawners);
}

void ABattleEventField::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if (m_Player == nullptr)
    {
        return;
    }

    /*どれか一つのフィールドコリジョンにプレイヤーが完全に入っているか*/
    FBox playerBox = m_Player->GetComponentsBoundingBox();
    for (UPrimitiveComponent* primComp : m_PrimitiveComponents)
    {
        if (primComp == nullptr)
        {
            continue;
        }

        FBox fieldBox = primComp->Bounds.GetBox();

        /*完全に入っていたらイベント開始*/
        if (fieldBox.IsInside(playerBox))
        {
            /*バトルイベント開始処理*/
            OnStartBattleEvent();
            break;
        }
    }
}

/**
 * @brief 自身のバトルフィールドの範囲(コリジョン)を取得する関数
 */
void ABattleEventField::GetPrimitiveComponents()
{
    m_PrimitiveComponents.Empty();

    /*自身のコリジョンコンポーネントを全て取得*/
    TArray<UPrimitiveComponent*> foundComponents;
    GetComponents<UPrimitiveComponent>(foundComponents);
    for (UPrimitiveComponent* comp : foundComponents)
    {
        /*コリンジョンが有効なものだけを取得*/
        if (comp != nullptr && comp->IsCollisionEnabled())
        {
            m_PrimitiveComponents.Add(comp);
        }
    }
}

/**
 * @brief 範囲内のスポナーを取得する
 */
void ABattleEventField::CollectSpawners()
{
    m_Spawners.Empty();
    m_RemainingEnemiesInField = 0;
    m_FieldPhases.Empty();  

    /*自身のUPrimitiveComponentを全て取得*/
    for (UPrimitiveComponent* primComp : m_PrimitiveComponents)
    {
        if (primComp == nullptr)
        {
            continue;
        }

        /*UPrimitiveComponent と Overlap しているスポナーを全て取得*/
        TArray<AActor*> overlappingSpawners;
        primComp->GetOverlappingActors(overlappingSpawners, AEnemySpawner::StaticClass());
        for (AActor* actor : overlappingSpawners)
        {
            if (AEnemySpawner* spawner = Cast<AEnemySpawner>(actor))
            {
                m_Spawners.Add(spawner);
                
                /*スポナーが持つフェーズを収集*/
                const TSet<ESpawnPhase> phases = spawner->GetSpawnPhases();
                for (const ESpawnPhase& phase : phases)
                {
                    /*フィールドに湧く敵の数をカウント*/
                    ++m_RemainingEnemiesInField;

                    /*すでにあれば入れない*/
                    if (!m_FieldPhases.Contains(phase))
                    {
                        m_FieldPhases.Add(phase);
                    }
                }
            }
        }

        /*ESpawnPhase の定義順にソート*/
        m_FieldPhases.Sort([](const ESpawnPhase& A, const ESpawnPhase& B) {
            return static_cast<uint8>(A) < static_cast<uint8>(B);
        });
    }

    /*一つもスポナーを取得できなかった*/
    if(m_Spawners.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleEventFiled failed to acquire any EnemySpawner instances."));
    }

    /*スポナーの取得完了通知*/
    OnSpawnersCollected.Broadcast(this);
}

/**
 * @brief イベントフィールドを稼働させる
 */
void ABattleEventField::ActiveEventField()
{
    if (m_IsActiveField)
    {
        return;
    }

    /*現在のフェーズ数を更新*/
    ++m_CurrentPhaseIndex;

    m_IsActiveField = true;
    /*スポナーに敵の生成を促す*/
    RequestSpawn();
}

/**
 * @brief バトルイベントが開始されるときの処理
 */
void ABattleEventField::OnStartBattleEvent()
{
    /*バリアエフェクト再生*/
    if(m_BarrierEffectComp != nullptr)
    {
        m_BarrierEffectComp->Activate(true); 
    }

    /*見えない壁(鳥かご)を有効*/
    ActiveCageCollision(true);
    /*Managerに通知*/
    OnBattleEventStart.Broadcast(this);

    m_EventTriggered = true;

    /*Tickを無効化*/
    SetActorTickEnabled(false);
}

/**
 * @brief バトルイベントが終了するときの処理
 */
void ABattleEventField::OnEndBattleEvent()
{
    if(!m_IsActiveField)
    {
        return;
    }
    
    /*バリアエフェクトの停止*/
    if(m_BarrierEffectComp != nullptr)
    {
        m_BarrierEffectComp->Deactivate();
    }
    /*見えない壁(鳥かご)を無効*/
    ActiveCageCollision(false);

    /*Managerに通知*/
    OnBattleEventEnd.Broadcast(this);
}

/**
 * @brief 見えない壁(鳥かご)の当たり判定を 有効 / 無効にする
 * 
 * @param 有効 or 無効
 */
void ABattleEventField::ActiveCageCollision(bool enable)
{
    for (TObjectPtr<ABattleEventCage> cage : m_CageActors)
    {
        if (cage == nullptr) 
        {
            continue;
        }

        cage->EnableCageCollision(enable);
    }
}

/**
 * @brief 範囲内のスポナーに対して敵の生成を促す
 */
void ABattleEventField::RequestSpawn()
{
    for (TObjectPtr<AEnemySpawner> spawner : m_Spawners)
    {
        if (spawner == nullptr)
        {
            continue;
        }

        /*スポナーが対象フェーズを持っていれば敵をスポーン*/
        if (m_FieldPhases.IsValidIndex(m_CurrentPhaseIndex - 1))
        {
            ESpawnPhase CurrentPhase = m_FieldPhases[m_CurrentPhaseIndex- 1];
            if (spawner->GetSpawnPhases().Contains(CurrentPhase))
            {
                /*スポナーに敵の生成を促し、敵死亡時のコールバック関数を引数で渡す*/
                FScriptDelegate delegate;
                delegate.BindUFunction(this, FName("OnEnemyDestroyed"));
                spawner->SpawnEnemy(delegate);

                /*敵の数をカウント*/
                ++m_RemainingEnemiesInPhase;
            }
        } 
    }
}

/**
 * @brief フェーズがスタートするときに処理
 */
void ABattleEventField::StartNextPhase()
{  
    /*敵キャラクターのスポーンを催促*/
    RequestSpawn();
    EventStartingPhase(m_CurrentPhaseIndex);

    /*フェーズ移行通知*/
    if(OnEndBattlePhase.IsBound())
    {
        OnEndBattlePhase.Broadcast(m_CurrentPhaseIndex);
    }
}

/**
 * @brief 敵が破棄された際に呼ばれる
 */
void ABattleEventField::OnEnemyDestroyed()
{    
    /*残りの敵キャラクターの数を減らす*/
    --m_RemainingEnemiesInField;
    --m_RemainingEnemiesInPhase;

    if (m_RemainingEnemiesInPhase <= 0)
    {
        /*次のフェーズがある*/
        if (m_CurrentPhaseIndex < m_FieldPhases.Num())
        {
            /*現在のフェーズ数を更新*/
            ++m_CurrentPhaseIndex;
            /*次のフェーズ開始*/
            StartNextPhase();
        }
        /*フィールド内の敵を全滅させた*/
        else
        {
            /*バトルイベント終了処理*/
            OnEndBattleEvent();
        }
    }
}

/** 
 * @brief オーバーラップ開始関数
 */
void ABattleEventField::OnFieldBeginOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor,UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
    /*すでにイベントが発動されていないなら処理*/
    if(!m_EventTriggered)
    {
        /*オーバーラップしたアクターがPlayerなら処理*/
        if (otherActor != nullptr && otherActor->ActorHasTag("Player"))
        {
            /*プレイヤーを一時的に保持しTick処理を稼働させる*/
            m_Player = otherActor;

            /*Tickを有効化しプレイヤーが完全にフィールド内に入ったかどうかを監視*/
            SetActorTickEnabled(true);
        }
    }
}