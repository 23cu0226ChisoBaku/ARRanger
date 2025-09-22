//*************************************************
// 敵キャラクターのスポナークラス
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "Public/BattleEvent/SpawnPhase.h"
#include "Delegates/DelegateCombinations.h"
#include "EnemySpawner.generated.h"

/*前方宣言*/
class FOnEnemyDestroyed;

DECLARE_MULTICAST_DELEGATE_OneParam(FGetSpawnActorDelegate, AActor*);

UCLASS()
class AEnemySpawner : public AActor
{
    GENERATED_BODY()
    
public:	
    AEnemySpawner();

    /**
     * @brief 敵キャラクターのスポーンを行う関数
     * 
     * @param スポーンした敵が破棄されたときに呼ぶデリゲート関数
     */
    void SpawnEnemy(const FScriptDelegate& onDestroyedCallback);

    /**
     * @brief スポナーの有効なフェーズを取得する 
     * 
     * @param 有効なフェーズ
     */
    TSet<ESpawnPhase> GetSpawnPhases() const { return m_SpawnPhases; }

    /**
     * @brief BP側からナイアガラを再生する
     */
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PlaySpawnEffect();

    /*スポーンしたアクターを受け渡す (for 麦)*/
    FGetSpawnActorDelegate GetSpawnedActorDelegate;

private:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AActor> m_SpawnEnemy;   /*スポーンさせる敵*/

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawner", meta = (AllowPrivateAccess="true"))
    TSet<ESpawnPhase> m_SpawnPhases;    /*スポーンフェーズ*/
};