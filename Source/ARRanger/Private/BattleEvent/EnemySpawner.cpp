//*************************************************
// 敵キャラクターのスポナークラス
//*************************************************

#include "Public/BattleEvent/EnemySpawner.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

// コンストラクタ
AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

/**
 * @brief 敵キャラクターのスポーンを行う関数
 * 
 * @param スポーンした敵が破棄されたときに呼ぶデリゲート関数
 */
void AEnemySpawner::SpawnEnemy(const FScriptDelegate& onDestroyedCallback)
{
    if (m_SpawnEnemy == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Enemy are set to be spawned by the EnemySpawner."));
        return;
    } 

    /*スポーンエフェクト再生*/
    PlaySpawnEffect();

    /*敵キャラクターをスポーンさせる*/
    AActor* enemyActor = GetWorld()->SpawnActor<AActor>(
        m_SpawnEnemy, 
        GetActorLocation(), 
        GetActorRotation()
    );

    if (enemyActor == nullptr)
    {
        return;
    }

    /*関数呼び出し*/
    GetSpawnedActorDelegate.Broadcast(enemyActor);
    /*デリゲートに登録*/
    enemyActor->OnDestroyed.Add(onDestroyedCallback);
}