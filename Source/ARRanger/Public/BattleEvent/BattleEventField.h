//*************************************************
// バトルイベントのフィールドクラス
//*************************************************
#pragma once

#include "CoreMinimal.h"
#include "EnemySpawner.h"
#include "Public/BattleEvent/SpawnPhase.h"
#include "BattleEventField.generated.h"

/*前方宣言*/
class AEnemySpawner;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBattleEventStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBattleEventEnded);

UCLASS()
class ABattleEventField : public AActor
{
    GENERATED_BODY()

public:
    ABattleEventField();
    virtual void BeginPlay() override;

    /**
     * @brief イベントフィールドを稼働させる
     */
    UFUNCTION(BlueprintCallable)
    void Active();

    /*イベント終了通知*/
    UPROPERTY(BlueprintAssignable, Category="BattleEvent")
    FBattleEventEnded OnBattleEventEnd;

private:  

    /**
     * @brief 自身のバトルフィールドの範囲(コリジョン)を取得する関数
     */
    void GetPrimitiveComponents();

    /**
     * @brief 範囲内のスポナーを取得する
     */
    void CollectSpawners();

    /**
     * @brief 次のフェーズがスタートするときに処理
     * 
     * @param 始めるフェーズ
     */
    void StartNextPhase(ESpawnPhase phase);

    /**
     * @brief 範囲内のスポナーに対して敵の生成を促す
     */
    void RequestSpawn();

    /**
     * @brief 敵が破棄された際に呼ばれる
     */
    void OnEnemyDestroyed();

    /** 
     * @brief オーバーラップ関数
     */
    void OnFieldBeginOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor,UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

    UPROPERTY()
    TSet<TObjectPtr<AEnemySpawner>> m_Spawners;                     /*範囲内にあるスポナー*/ 
    UPROPERTY()
    TSet<TObjectPtr<UPrimitiveComponent>> m_PrimitiveComponents;    /*バトルフィールドのコリンジョンコンポーネント*/
    UPROPERTY()
    TArray<ESpawnPhase> m_FieldPhases;                              /*フィールドのスポーンフェーズ*/
    UPROPERTY()
    int32 m_CurrentPhaseIndex;                                      /*現在のフェーズ*/
    UPROPERTY()
    int32 m_RemainingEnemiesInPhase;                                /*現フェーズの残り敵数*/
};