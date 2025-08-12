#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "InsekiGameMode.generated.h"

class ABlinkOutlineTickActor;

// 麦
namespace ARRanger
{
  struct INotifyHandlerInterface;
}
class IObservableSubjectInterface;

UCLASS()
class ARRANGER_API AInsekiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AInsekiGameMode();

protected:
	virtual void BeginPlay() override;

	// ゲームクリア時に呼び出される関数
	void HandleGameClear();

private:
  void InitializeObserver();

public:
	// �G�����񂾂Ƃ��ɌĂяo�����
	void OnEnemyKilled();

	// �G�̐�
	UPROPERTY(EditAnywhere, Category = "Game")
	int32 EnemyCount;

	// �Q�[���N���A�̃��[�U�[�E�B�W�F�b�g
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
	TSubclassOf<ABlinkOutlineTickActor> BlinkTickActorClass;

};
