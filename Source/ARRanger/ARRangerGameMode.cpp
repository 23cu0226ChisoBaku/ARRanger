#include "ARRangerGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AARRangerGameMode::AARRangerGameMode()
{
	
}

void AARRangerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// タグ付きの敵をすべて取得
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);

	EnemyCount = FoundEnemies.Num();
}

void AARRangerGameMode::OnEnemyKilled()
{
	--EnemyCount;

	UE_LOG(LogTemp, Warning, TEXT("Enemy Count: %d"), EnemyCount);

	if (EnemyCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Clear!"));

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC && GameClearWidgetClass)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());

			// 少し待ってからゲームクリア処理へ
			FTimerHandle ClearTimerHandle;
			GetWorldTimerManager().SetTimer(ClearTimerHandle, this, &AARRangerGameMode::HandleGameClear, 3.0f, false);
		}
	}
}

void AARRangerGameMode::HandleGameClear()
{
	// プレイヤー操作停止
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
		PC->SetShowMouseCursor(true);
	}

	// レベル遷移
	UGameplayStatics::OpenLevel(this, FName("GameClear"));
}