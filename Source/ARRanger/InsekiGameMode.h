#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InsekiGameMode.generated.h"

UCLASS()
class ARRANGER_API AInsekiGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

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
