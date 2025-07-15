#include "InsekiGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AInsekiGameMode::BeginPlay()
{
	Super::BeginPlay();

	// ƒ^ƒO•t‚«‚Ì“G‚ð‚·‚×‚ÄŽæ“¾
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);

	EnemyCount = FoundEnemies.Num();

	UE_LOG(LogTemp, Warning, TEXT("Initial Enemy Count: %d"), EnemyCount);
}

void AInsekiGameMode::OnEnemyKilled()
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

			UUserWidget* Widget = CreateWidget(PC, GameClearWidgetClass);
			if (Widget) Widget->AddToViewport();
		}
	}
}