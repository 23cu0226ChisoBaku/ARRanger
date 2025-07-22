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
	// 敵が死んだときに呼び出される
	void OnEnemyKilled();

	// 敵の数
	UPROPERTY(EditAnywhere, Category = "Game")
	int32 EnemyCount;

	// ゲームクリアのユーザーウィジェット
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameClearWidgetClass;
};
