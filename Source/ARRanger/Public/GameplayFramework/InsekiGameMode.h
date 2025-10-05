#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "InsekiGameMode.generated.h"

#define UE_API ARRANGER_API

class AOutlineTickActor;

namespace Private
{
  struct FARGameUserSettingsData;
}

// 麦
namespace ARRanger
{
  struct INotifyHandlerInterface;
}

class IObservableSubjectInterface;

UCLASS()
class AInsekiGameMode : public AGameModeBase
{
	GENERATED_BODY()

enum EGameResultState
{
  GameClear,
  GameOver,
};

public:
	UE_API AInsekiGameMode();

protected:
	UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
  void InitializeObserver();
  void InitializeEvents();
  void ProcessGameResult(EGameResultState ResultState);
  void OnResetCommandSent();
  void SetGameUserSettings(TArray<Private::FARGameUserSettingsData>& OutSettingsDataStack);

  // Enemy Initialization
  void InitializeOnMapEnemies();
  void UninitializeAliveEnemies();
  void UninitializeEvents();
  void UnsetGameUserSettings(TArray<Private::FARGameUserSettingsData>& OutSettingsDataStack);

  UFUNCTION()
  void ARGameOver();

public:

  UFUNCTION()
	UE_API void OnEnemyDead(AActor* InEnemy);

private:
	UPROPERTY(EditAnywhere, Category = "Game")
	int32 EnemyCount;

public:

	// 麦
	UPROPERTY(EditDefaultsOnly, Category = "AR|Test|SoundData", meta = (RequiredAssetDataTags = "RowStructure=/Script/ARRanger.ARSoundMetaData"))
	TObjectPtr<UDataTable> SoundEffectData;

	// 麦
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

  // TODO Temporary
  UPROPERTY(EditDefaultsOnly)
  TSubclassOf<class AARPhysicsTickProcessorActor> ProcessorActorClass;

	TSharedPtr<ARRanger::INotifyHandlerInterface> NotifyHandler;
	TArray< TWeakInterfacePtr< IObservableSubjectInterface > > Subjects;

	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	TSubclassOf<AOutlineTickActor> OutlineTickActorClass;

  UPROPERTY()
  TObjectPtr<AActor> BossPtr;

  uint8 bGameResultHandled : 1;

  FTimerHandle GameResultTimerHandle;

};

#undef UE_API
