#include "ARRangerGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AARRangerGameMode::AARRangerGameMode()
{
	// stub
}

void AARRangerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// タグ付きの敵をすべて取得
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);

	EnemyCount = FoundEnemies.Num();

	UE_LOG(LogTemp, Warning, TEXT("Initial Enemy Count: %d"), EnemyCount);
}

void AARRangerGameMode::OnEnemyKilled()
{
	EnemyCount--;

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

	// UI表示
	if (GameClearWidgetClass)
	{
		UUserWidget* ClearWidget = CreateWidget<UUserWidget>(GetWorld(), GameClearWidgetClass);
		if (ClearWidget)
		{
			ClearWidget->AddToViewport();
		}
	}
}