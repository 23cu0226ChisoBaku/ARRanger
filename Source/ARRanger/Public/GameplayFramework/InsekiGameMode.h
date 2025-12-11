/**
 * @file InsekiGameMode.h
 * @author MAI ZHICONG
 * @brief メインゲームモード
 */

#pragma once

#include "GameFramework/GameModeBase.h"

#include "InsekiGameMode.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class AOutlineTickActor;
class AARPhysicsTickProcessorActor;
class IObservableSubjectInterface;
namespace Private
{
  struct FARGameUserSettingsData;
}
namespace ARRanger
{
  struct INotifyHandlerInterface;
}

UCLASS()
class AInsekiGameMode : public AGameModeBase
{
	GENERATED_BODY()

/**
 * @brief ゲーム結果ステート列挙（内部）
 */
enum EGameResultState
{
  GameClear,
  GameOver,
};

public:
	UE_API AInsekiGameMode();

protected:
  /**Start AActor Interface */
	UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  /**End AActor Interface */

private:

  /**オブサーバーを初期化 */
  void InitializeObserver();

  /**イベントを初期化 */
  void InitializeEvents();

  /**AR物理システムを初期化 */
  void InitializeARPhysics();

  /**
   * @brief ゲームリザルトメッセージを処理する
   * 
   * @param ResultState リザルトメッセージ
   */
  void ProcessGameResult(EGameResultState ResultState);

  /**リセットコマンド送ったコールバック */
  void OnResetCommandSent();

  /**ゲームユーザーセッティングを設定する */
  void SetGameUserSettings(TArray<Private::FARGameUserSettingsData>& OutSettingsDataStack);
  
  /**ゲームユーザーセッティングをリセットする */
  void ResetGameUserSettings(TArray<Private::FARGameUserSettingsData>& OutSettingsDataStack);
  /**マップにいる敵を初期化する */
  void InitializeOnMapEnemies();

  /**生きている敵の初期化を解除する */
  void UninitializeAliveEnemies();

  /**イベント初期化を解除する */
  void UninitializeEvents();

  /**AR物理システム初期化を解除する */
  void UninitializeARPhysics();

  UFUNCTION()
  void HandleGameOver();

  UFUNCTION()
	void OnEnemyDead(AActor* InEnemy);

private:
	UPROPERTY(EditAnywhere, Category = "Game")
	int32 EnemyCount;

public:

	UPROPERTY(EditDefaultsOnly, Category = "AR|Test|SoundData", meta = (RequiredAssetDataTags = "RowStructure=/Script/ARRanger.ARSoundMetaData"))
	TObjectPtr<UDataTable> SoundEffectData;

private:

	/**
	 * @brief バトルイベントが開始した際の処理
	 */
	UFUNCTION()
	void OnStartBattleEvent();

	/**
	 * @brief バトルイベントが終了した際の処理
	 */
	UFUNCTION()
	void OnEndBattleEvent();

	void RegisterBattleEventDelegate();

	void UnregisterBattleEventDelegate();

  void OnEnemySpawned(AActor* InSpawnedEnemy);

  UPROPERTY(EditDefaultsOnly)
  TSubclassOf<AARPhysicsTickProcessorActor> ProcessorActorClass;

  UPROPERTY()
  TObjectPtr<AARPhysicsTickProcessorActor> ProcessorActor;

	TSharedPtr<ARRanger::INotifyHandlerInterface> NotifyHandler;
	TArray< TWeakInterfacePtr< IObservableSubjectInterface > > Subjects;

	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	TSubclassOf<AOutlineTickActor> OutlineTickActorClass;

  UPROPERTY()
  TObjectPtr<AActor> BossPtr;

  FTimerHandle GameResultTimerHandle;
  
  uint8 bGameResultHandled : 1;
};

#undef UE_API
