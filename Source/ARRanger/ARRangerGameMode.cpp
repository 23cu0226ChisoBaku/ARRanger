#include "ARRangerGameMode.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AARRangerGameMode::AARRangerGameMode()
{
	// stub
}

void AARRangerGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		FString OutError;
		ULocalPlayer* NewPlayer = GameInstance->CreateLocalPlayer(1, OutError, true);

		if (!NewPlayer)
		{
			UE_LOG(LogTemp, Warning, TEXT("2Pの作成に失敗: %s"), *OutError);
		}
	}

	// タグ付きの敵をすべて取得
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);

	EnemyCount = FoundEnemies.Num();
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