#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ARRangerGameMode.generated.h"

UCLASS(abstract)
class AARRangerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AARRangerGameMode();

protected:
	virtual void BeginPlay() override;

	// �Q�[���N���A���ɌĂяo�����֐�
	void HandleGameClear();

public:
	// �G�����񂾂Ƃ��ɌĂяo�����
	void OnEnemyKilled();

	// �G�̐�
	UPROPERTY(EditAnywhere, Category = "Game")
	int32 EnemyCount;

	// �Q�[���N���A�̃��[�U�[�E�B�W�F�b�g
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameClearWidgetClass;
};