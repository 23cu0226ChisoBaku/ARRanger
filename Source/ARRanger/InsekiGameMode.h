#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "InsekiGameMode.generated.h"

#define UE_API ARRANGER_API

class AOutlineTickActor;

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

public:
	UE_API AInsekiGameMode();

protected:
	UE_API virtual void BeginPlay() override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
  void InitializeObserver();
  void InitializeEvents();
  void ProcessGameClear();
  void OnResetCommandSent();

public:

	UE_API void OnEnemyKilled(AActor* KilledEnemy);

	UPROPERTY(EditAnywhere, Category = "Game")
	int32 EnemyCount;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameClearWidgetClass;

	// 麦
	UPROPERTY(EditDefaultsOnly, Category = "AR|Test|SoundData", meta = (RequiredAssetDataTags = "RowStructure=/Script/ARRanger.ARSoundMetaData"))
	TObjectPtr<UDataTable> SoundEffectData;

	// 麦
private:

  // TODO Temporary
  UPROPERTY(EditDefaultsOnly)
  TSubclassOf<class AARPhysicsTickProcessorActor> ProcessorActorClass;

	TSharedPtr<ARRanger::INotifyHandlerInterface> NotifyHandler;
	TArray< TWeakInterfacePtr< IObservableSubjectInterface > > Subjects;

	UPROPERTY(EditDefaultsOnly, Category = "Blinking")
	TSubclassOf<AOutlineTickActor> OutlineTickActorClass;

  UPROPERTY()
  TObjectPtr<AActor> BossPtr;

  uint8 bGameClearHandled : 1;

  FTimerHandle GameClearTimerHandle;

};

#undef UE_API
