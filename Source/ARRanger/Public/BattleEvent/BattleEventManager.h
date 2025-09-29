//*************************************************
// バトルイベントの管理クラス
//*************************************************

#pragma once
#include "CoreMinimal.h" 
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"

#include "BattleEventManager.generated.h"

/*前方宣言*/ 
class ABattleEventField;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBattleEventStart); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBattleEventEnd);

UCLASS() 
class ABattleEventManager : public AActor 
{ 
    GENERATED_BODY()

public: 
    ABattleEventManager(); 
    virtual void BeginPlay() override; 
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    /*フィールド開始通知 (外部クラス用)*/
    UPROPERTY(BlueprintAssignable)
    FBattleEventStart OnAnyFieldBattleStart;
    /*フィールド終了通知 (外部クラス用)*/
    UPROPERTY(BlueprintAssignable)
    FBattleEventEnd OnAnyFieldBattleEnd;

    /** 
     * @brief 進行中のフィールドの残りの敵数を取得 
     * 
     * @return 進行中のフィールドの残りの敵数 
     */ 
    UFUNCTION(BlueprintCallable, Category = "BattleEvent") 
    int32 GetCurrentRemainingEnemiesInField() const;

    /** 
     * @brief 進行中のフェーズの残りの敵数を取得
     * 
     * @return 進行中のフェーズの残りの敵数 
     */ 
    UFUNCTION(BlueprintCallable, Category = "BattleEvent") 
    int32 GetCurrentRemainingEnemiesInPhase() const;

    // バリアエフェクト出現時のSE
    UPROPERTY(EditAnywhere, Category = "SE")
    TObjectPtr<USoundBase> SE_Barrier_Start;
    // バリアエフェクト消失時のSE
    UPROPERTY(EditAnywhere, Category = "SE")
    TObjectPtr<USoundBase> SE_Barrier_End;

private:

    /**
     * @brief 現在のバトルフィールドを取得する
     * 
     * @return 現在のバトルフィールド
     */
    UFUNCTION(BlueprintCallable)
    ABattleEventField* GetCurrentBattleField() const { return m_CurrentBattleField;}

    /**
     * @brief レベル上のフィールドを取得する
     * 
     * @return レベル上のフィールド
     */
    UFUNCTION(BlueprintCallable)
    TArray<ABattleEventField*> GetAllBattleField() const { return m_BattleFields;}

    /**
     * @brief バトルフィールドがスポナーを収集完了
     * 
     * @param 範囲内のスポナーの収集が終わったフィールド
     */
    UFUNCTION()
    void HandleFieldSpawnersCollected(ABattleEventField* field = nullptr);

    /**
     * @brief 一番最初とインデックス分のフィールドをアクティブ化
     */
    UFUNCTION()
    void BeginInitialBattleEvent();

    /** 
     * @brief バトルイベント開始時の処理 
     * 
     * @param バトルイベントが開始したフィールド 
     */ 
    UFUNCTION() 
    void HandleBattleEventStart(ABattleEventField* startedBattleField);

    /**
     * @brief バトルイベント終了時の処理 
     * 
     * @param バトルイベントが終了したフィールド
     */ 
    UFUNCTION() 
    void HandleBattleEventEnd(ABattleEventField* finishedBattleField);

    /**
     * @brief レベル上の全てのバトルフィールドが初期化済みかどうかを返す
     * 
     * @return レベル上の全てのバトルフィールドが初期化済みかどうか
     */
    bool IsAllFieldsInitialized() const ;

    /** 
     * @brief 次のフィールドをオフセット分稼働させる 
     */ 
    void ActivateNextFields();

    /**
     * @brief 指定のフィールドを稼働させる
     * 
     * @param 稼働させるフィールド 
     */ 
    void SetActiveField(ABattleEventField* newField);

    UPROPERTY(EditAnywhere, Category = "BattleEvent", meta=(AllowPrivateAccess="true")) 
    TArray<TObjectPtr<ABattleEventField>> m_BattleFields;   /*レベル内の全てのBattleEventField*/ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleEvent", meta=(AllowPrivateAccess="true")) 
    int32 m_ActiveFieldOffset = 1;                          /*アクティブ化させるフィールドインデックス数*/ 

    UPROPERTY()
    TSet<ABattleEventField*> m_InitializedFields;           /*初期化が終わったフィールド*/
    UPROPERTY() 
    TArray<TObjectPtr<ABattleEventField>> m_ActiveFields;   /*現在アクティブなフィールド*/ 
    UPROPERTY() 
    TObjectPtr<ABattleEventField> m_CurrentBattleField;     /*プレイヤーが現在戦っているフィールド*/
};