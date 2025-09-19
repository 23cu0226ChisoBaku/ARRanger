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

    /*OnDestroyed デリゲートに登録*/
    enemyActor->OnDestroyed.Add(onDestroyedCallback);
}