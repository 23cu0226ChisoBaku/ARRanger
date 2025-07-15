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

	// ゲームクリア時に呼び出される関数
	void HandleGameClear();

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