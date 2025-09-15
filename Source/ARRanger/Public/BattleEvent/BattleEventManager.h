//*************************************************
// バトルイベントの管理クラス
//*************************************************

#pragma once
#include "CoreMinimal.h" 
#include "GameFramework/Actor.h"

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

private:

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
    int32 m_nextFieldOffset = 1;                            /*アクティブ化させるフィールドインデックス数*/ 

    UPROPERTY() 
    TObjectPtr<ABattleEventField> m_ActiveField;            /*現在アクティブなフィールド*/ 
    UPROPERTY() 
    TObjectPtr<ABattleEventField> m_CurrentBattleField;     /*プレイヤーが現在戦っているフィールド*/ 
};