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
class ABattleEventCage;
class UNiagaraComponent;
class ANiagaraActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndBattlePhase, int32, currentPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnersCollected, ABattleEventField*, field);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBattleEventStarted, ABattleEventField*, startedBattleField);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBattleEventEnded, ABattleEventField*, finishedBattleField);

UCLASS()
class ABattleEventField : public AActor
{
    GENERATED_BODY()

public:
    ABattleEventField();
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    /**
     * @brief イベントフィールドを稼働させる
     */
    UFUNCTION(BlueprintCallable)
    void ActiveEventField();

    /**
     * @brief フィールドがアクティブになったかどうかを返す
     * 
     * @return フィールドがアクティブになったかどう
     */
    bool IsActivedField() const { return m_IsActiveField; }

    /** 
     * @brief 現在のフィールドの残りの敵数を取得 
     * 
     * @return 現在のフィールドの残りの敵数 
     */ 
    int32 GetRemainingEnemiesInField() const { return m_RemainingEnemiesInField;}

    /** 
     * @brief 現在のフェーズの残りの敵数を取得 
     * 
     * @return 現在のフェーズの残りの敵数 
     */ 
    int32 GetRemainingEnemiesInPhase() const { return m_RemainingEnemiesInPhase;}

    /*妥協処理*/
    UFUNCTION(BlueprintImplementableEvent)
    void EventEndingPhase(int32 currentPhase);

    UFUNCTION(BlueprintImplementableEvent)
    void EventStartingPhase(int32 currentPhase);   
    
    /*スポナー収集完了通知*/
    UPROPERTY(BlueprintAssignable, Category="BattleEvent")
    FOnSpawnersCollected OnSpawnersCollected;

    /*イベント開始通知*/
    UPROPERTY(BlueprintAssignable, Category="BattleEvent")
    FBattleEventStarted OnBattleEventStart;

    /*イベント終了通知*/
    UPROPERTY(BlueprintAssignable, Category = "BattleEvent")
    FBattleEventEnded OnBattleEventEnd;

    /*フェーズ終了通知*/
    UPROPERTY(BlueprintAssignable, Category = "BattleEvent")
    FEndBattlePhase OnEndBattlePhase;

private:  

    /**
     * @brief 敵が破棄された際に呼ばれる
     */
    UFUNCTION()
    void OnEnemyDestroyed();

    /** 
     * @brief オーバーラップ関数
     */
    UFUNCTION()
    void OnFieldBeginOverlap(UPrimitiveComponent* overlappedComp, AActor* otherActor,UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

    /**
     * @brief 自身のバトルフィールドの範囲(コリジョン)を取得する関数
     */
    void GetPrimitiveComponents();

    /**
     * @brief 範囲内のスポナーを取得する
     */
    void CollectSpawners();

    /**
     * @brief 見えない壁(鳥かご)の当たり判定を 有効 / 無効にする
     * 
     * @param 有効 or 無効
     */
    void ActiveCageCollision(bool enable);

    /**
     * @brief 次のフェーズがスタートするときに処理
     */
    void StartNextPhase();

    /**
     * @brief 範囲内のスポナーに対して敵の生成を促す
     */
    void RequestSpawn();

    /**
     * @brief バトルイベントが開始されるときの処理
     */
    void OnStartBattleEvent();

    /**
     * @brief バトルイベントが終了するときの処理
     */
    void OnEndBattleEvent();


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cage", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<ABattleEventCage>> m_CageActors;              /*見えない壁(鳥かご)*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ANiagaraActor> m_BarrierEffectActor;                 /*バリアのエフェクトのナイアガラ*/

    UPROPERTY()
    TSet<TObjectPtr<AEnemySpawner>> m_Spawners;                     /*範囲内にあるスポナー*/ 
    UPROPERTY()
    TSet<TObjectPtr<UPrimitiveComponent>> m_PrimitiveComponents;    /*バトルフィールドのコリンジョンコンポーネント*/
    UPROPERTY()
    TArray<ESpawnPhase> m_FieldPhases;                              /*フィールドのスポーンフェーズ*/
    UPROPERTY()
    TObjectPtr<AActor> m_Player;                                    /*プレイヤー*/
    UPROPERTY()
    TObjectPtr<UNiagaraComponent> m_BarrierEffectComp;              /*バリアエフェクトのコンポーネント*/
    UPROPERTY()
    int32 m_CurrentPhaseIndex;                                      /*現在のフェーズ*/
    UPROPERTY()
    int32 m_RemainingEnemiesInField;                                /*現フィールドの残り敵数*/
    UPROPERTY()
    int32 m_RemainingEnemiesInPhase;                                /*現フェーズの残り敵数*/
    UPROPERTY()
    bool m_IsActiveField;                                           /*フィールドをアクティブにしたかどうか*/
    UPROPERTY()
    bool m_EventTriggered;                                          /*イベントとが発動されたか*/
};