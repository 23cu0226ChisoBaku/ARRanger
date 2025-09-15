//*************************************************
// バトルイベントのフィールドクラス
//*************************************************

#include "public/BattleEvent/BattleEventField.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "Public/BattleEvent/EnemySpawner.h"
#include "Public/BattleEvent/BattleEventCage.h"

ABattleEventField::ABattleEventField()
    : m_Player(nullptr)                
    , m_RemainingEnemiesInField(0)     
    , m_CurrentPhaseIndex(0)
    , m_RemainingEnemiesInPhase(0)
    , m_IsActiveField(false)
    , m_EventTriggered(false)

{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleEventField::BeginPlay()
{
    Super::BeginPlay();

    /*バトルフィールド範囲用（コリジョン）コンポーネントを全て取得*/
    GetPrimitiveComponents();

    /*範囲内のスポナーを取得*/
    //CollectSpawners();

    /*各範囲コリンジョンコンポーネントににオーバーラップ関数をバインド*/
    for (UPrimitiveComponent* primComp : m_PrimitiveComponents)
    {
        if (primComp == nullptr) 
        {
            continue;
        }
        primComp->OnComponentBeginOverlap.AddDynamic(this, &ABattleEventField::OnFieldBeginOverlap);
    }
}

void ABattleEventField::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    
    if (m_Player)
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
            /*バトルイベント終了処理*/
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

    UE_LOG(LogTemp, Warning, TEXT("ABattleEventField::CollectSpawners()"));

    /*自身のUPrimitiveComponentを全て取得*/
    for (UPrimitiveComponent* primComp : m_PrimitiveComponents)
    {
        if (primComp == nullptr)
        {
            continue;
        }

        UE_LOG(LogTemp, Warning, TEXT("ABattleEventField::CollectSpawners() : Field have PrimitiveComponent"));

        /*UPrimitiveComponent と Overlap しているスポナーを全て取得*/
        TArray<AActor*> overlappingSpawners;
        primComp->GetOverlappingActors(overlappingSpawners, AEnemySpawner::StaticClass());
        for (AActor* actor : overlappingSpawners)
        {
            UE_LOG(LogTemp, Warning, TEXT("ABattleEventField::CollectSpawners() : overlapping anything"));

            if (AEnemySpawner* spawner = Cast<AEnemySpawner>(actor))
            {
                UE_LOG(LogTemp, Warning, TEXT("ABattleEventField::CollectSpawners() : overlapping Spawner"));

                m_Spawners.Add(spawner);

                /*フィールドに湧く敵の数をカウント*/
                ++m_RemainingEnemiesInField;

                /*スポナーが持つフェーズを収集*/
                const TArray<ESpawnPhase> phases = spawner->GetSpawnPhases();
                for (const ESpawnPhase& phase : phases)
                {
                    m_FieldPhases.Add(phase);
                }
            }
        }
    }
}

/**
 * @brief イベントフィールドを稼働させる
 */
void ABattleEventField::ActiveEventField()
{
    /*スポナーに敵の生成を促す*/
    RequestSpawn();
    m_IsActivedField = true;
}

/**
 * @brief バトルイベントが開始されるときの処理
 */
void ABattleEventField::OnStartBattleEvent()
{
    /*見えない壁(鳥かご)を有効*/
    ActiveCageCollision(true);
    /*Managerに通知*/
    OnBattleEventStart.Broadcast(this);

    m_EventTriggered = true;
    PrimaryActorTick.bCanEverTick = false;
}

/**
 * @brief バトルイベントが終了するときの処理
 */
void ABattleEventField::OnEndBattleEvent()
{
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
    UE_LOG(LogTemp, Warning, TEXT("ABattleEventField::RequestSpawn() : m_Spawners not found EnemySpawner"));
    for (TObjectPtr<AEnemySpawner> spawner : m_Spawners)
    {
        if (spawner == nullptr)
        {
            continue;
        }

        /*スポナーに敵の生成を促し、敵死亡時のコールバック関数を引数で渡す*/
        FScriptDelegate delegate;
        delegate.BindUFunction(this, FName("OnEnemyDestroyed"));
        spawner->SpawnEnemy(delegate);

        /*敵の数をカウント*/
        ++m_RemainingEnemiesInPhase;

        UE_LOG(LogTemp, Warning, TEXT("ABattleEventField::RequestSpawn()残りの敵: %d"), m_RemainingEnemiesInPhase);
    }
}

/**
 * @brief フェーズがスタートするときに処理
 * 
 * @param 始めるフェーズ
 */
void ABattleEventField::StartNextPhase(ESpawnPhase phase)
{
    for (TObjectPtr<AEnemySpawner> spawner : m_Spawners)
    {
        if (spawner == nullptr)
        {
            continue;
        }
            
        /*スポナーが対象フェーズを持っていれば敵をスポーン*/
        if (spawner->GetSpawnPhases().Contains(phase))
        {
            RequestSpawn();
        }
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
        /*現在のフェーズが終了した*/
        ++m_CurrentPhaseIndex;

        /*次のフェーズがある*/
        if (m_CurrentPhaseIndex < m_FieldPhases.Num())
        {
            /*スポーン開始*/
            StartNextPhase(m_FieldPhases[m_CurrentPhaseIndex]);
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
            PrimaryActorTick.bCanEverTick = true;
        }
    }
}