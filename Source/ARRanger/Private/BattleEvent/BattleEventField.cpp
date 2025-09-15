//*************************************************
// バトルイベントのフィールドクラス
//*************************************************

#include "public/BattleEvent/BattleEventField.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "Public/BattleEvent/EnemySpawner.h"

ABattleEventField::ABattleEventField()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleEventField::BeginPlay()
{
    Super::BeginPlay();

    /*バトルフィールド範囲用（コリジョン）コンポーネントを全て取得*/
    GetPrimitiveComponents();

    /*範囲内のスポナーを取得*/
    CollectSpawners();

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
void ABattleEventField::Active()
{
    /*フェーズをカウントする*/
    ++m_CurrentPhaseIndex;
    /*スポナーに敵の生成を促す*/
    RequestSpawn();
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

        /*スポナーに敵の生成を促し、敵死亡時のコールバック関数を引数で渡す*/
        FScriptDelegate delegate;
        delegate.BindUFunction(this, FName("OnEnemyDestroyed"));
        spawner->SpawnEnemy(delegate);

        /*敵の数をカウント*/
        ++m_RemainingEnemiesInPhase;
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
            
        /*スポナーが対象フェーズを持っていれば敵を出す*/
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
            /*Managerに通知*/
            //OnBattleEventEnd
        }
    }
}

/** 
 * @brief オーバーラップ関数
 */
void ABattleEventField::OnFieldBeginOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor,UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
    if (otherActor && otherActor->ActorHasTag("Player"))
    {
        FBox fieldBox = overlappedComp->Bounds.GetBox();
        FBox playerBox = otherActor->GetComponentsBoundingBox();

        if (fieldBox.IsInside(playerBox))
        {
            /*鳥かごを作る*/
        }
    }
}