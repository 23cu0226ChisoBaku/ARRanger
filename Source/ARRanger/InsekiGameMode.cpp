#include "InsekiGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "AudioSystem/ARAudioSystem.h"

#include "PlayerObservation/PlayerNotifyHandler.h"
#include "PlayerObservation/IObservableSubjectInterface.h"
#include "PlayerObservation/ObserverListNode.h"

#include "AudioSystem/ARSoundPlayLibrary.h"

void AInsekiGameMode::BeginPlay()
{
	Super::BeginPlay();

	// �^�O�t���̓G�����ׂĎ擾
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundEnemies);

	EnemyCount = FoundEnemies.Num();

	UE_LOG(LogTemp, Warning, TEXT("Initial Enemy Count: %d"), EnemyCount);

  // 音声データを初期化
  const UWorld* world = GetWorld();
  if (world != nullptr)
  {
    UARAudioSystem* audioSystem = world->GetGameInstance()->GetSubsystem<UARAudioSystem>();
    if (audioSystem != nullptr)
    {
      audioSystem->InitializeSounds(/**BGM */ nullptr, /**SE */ SoundEffectData);
    }
  }

  InitializeObserver();
}

void AInsekiGameMode::InitializeObserver()
{
  using ARRanger::Player::FPlayerNotifyHandler;
  using ARRanger::FObserverListNode;
  using ARRanger::FObserverListRootNode;

  // プレイヤーのオブサーバーイベントハンドラーを作成
  ACharacter* player = UGameplayStatics::GetPlayerCharacter(this, 0);
  if (player != nullptr && player->GetClass()->ImplementsInterface(UObservableSubjectInterface::StaticClass()))
  {
    TSharedPtr<FPlayerNotifyHandler> playerHandler = ::MakeShared<FPlayerNotifyHandler>();
    
    // 攻撃
    TSharedPtr<FObserverListNode> attackObserverListNode = FObserverListRootNode::MakeListNode();
    attackObserverListNode->Initialize();
    attackObserverListNode->BindNewObserver(FSimpleDelegate::CreateLambda([this]() 
    { 
      ACharacter* player = UGameplayStatics::GetPlayerCharacter(this, 0);
      if (player != nullptr)
      {
        FARSoundEffectParameters params;
        params.Dimension = EARSoundDimension::Dimension3;
        params.Pitch = 1.0f;
        params.PlayLocation = player->GetActorLocation();
        params.RepositoryAccessActor = player;
        params.SEType = EARSoundEffectType::Attack;
  
        UARSoundPlayLibrary::PlaySESound(params); 
      }
    }));

    // ジャンプ
    TSharedPtr<FObserverListNode> jumpObserverListNode = FObserverListRootNode::MakeListNode();
    jumpObserverListNode->Initialize();
    jumpObserverListNode->BindNewObserver(FSimpleDelegate::CreateLambda([this]() 
    { 
      ACharacter* player = UGameplayStatics::GetPlayerCharacter(this, 0);
      if (player != nullptr)
      {
        FARSoundEffectParameters params;
        params.Dimension = EARSoundDimension::Dimension3;
        params.Pitch = 1.0f;
        params.PlayLocation = player->GetActorLocation();
        params.RepositoryAccessActor = player;
        params.SEType = EARSoundEffectType::Jump;
  
        UARSoundPlayLibrary::PlaySESound(params); 
      }
    }));

    // ダッシュ
    TSharedPtr<FObserverListNode> dashObserverListNode = FObserverListRootNode::MakeListNode();
    dashObserverListNode->Initialize();
    dashObserverListNode->BindNewObserver(FSimpleDelegate::CreateLambda([this]() 
    { 
      ACharacter* player = UGameplayStatics::GetPlayerCharacter(this, 0);
      if (player != nullptr)
      {
        FARSoundEffectParameters params;
        params.Dimension = EARSoundDimension::Dimension3;
        params.Pitch = 5.0f;
        params.PlayLocation = player->GetActorLocation();
        params.RepositoryAccessActor = player;
        params.SEType = EARSoundEffectType::Dash;
  
        UARSoundPlayLibrary::PlaySESound(params); 
      }
    }));
  
    playerHandler->AttackObserverRoot.AttachNewObserverList(attackObserverListNode);
    playerHandler->DashObserverRoot.AttachNewObserverList(dashObserverListNode);
    playerHandler->JumpObserverRoot.AttachNewObserverList(jumpObserverListNode);

    // Handlerを更新する
    Cast<IObservableSubjectInterface>(player)->UpdateNotifyHandler(::StaticCastSharedPtr<ARRanger::INotifyHandlerInterface>(playerHandler));
    NotifyHandler = playerHandler;
  }  

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