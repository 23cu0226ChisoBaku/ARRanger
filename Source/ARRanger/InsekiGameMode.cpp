#include "InsekiGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "AudioSystem/ARAudioSystem.h"

#include "PlayerObservation/PlayerNotifyHandler.h"
#include "PlayerObservation/IObservableSubjectInterface.h"
#include "PlayerObservation/ObserverListNode.h"

#include "AudioSystem/ARSoundPlayLibrary.h"

#include "PlayerObservation/IObserverRegistry.h"
#include "PlayerObservation/Registry/SoundEffectRegistry.h"

#include "Physics/Gameplay/ARPhysicsGlobal.h"
#include "Physics/Core/ARPhysicsTickProcessorActor.h"
#include "Public/BlinkingSystem/BlinkingOutlineWorldSubsystem.h"


// TODO May move initialize function to another file
#include "Physics/IARPhysicsSystemHost.h"

AInsekiGameMode::AInsekiGameMode()
{
    ProcessorActorClass = AARPhysicsTickProcessorActor::StaticClass();
}


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

  // BlinkingOutlineWorldSubsystem を取得
	UBlinkingOutlineWorldSubsystem* WorldSubsystem = GetWorld()->GetSubsystem<UBlinkingOutlineWorldSubsystem>();
	if (WorldSubsystem != nullptr)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC != nullptr && PC->GetPawn() != nullptr)
		{
			// プレイヤーのPawnからLineTraceSingleARObjectComponentを取得
			UDetectorMagnetizableComponent* DetectorMagnetizableComp = PC->GetPawn()->FindComponentByClass<UDetectorMagnetizableComponent>();

			// TickActorClassを設定し、サブシステムにセットアップを依頼
			WorldSubsystem->SetupBlinkingSystem(GetWorld(), DetectorMagnetizableComp, OutlineTickActorClass);
		}
	}

  InitializeObserver();

  // 物理システム初期化
  ARRanger::Private::FARPhysicsCore::InitializeARPhysicsInWorldWithActorType(GetWorld(), ProcessorActorClass);
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

    {
      FARSoundEffectParameters attackSEParam;
      attackSEParam.Dimension = EARSoundDimension::Dimension3;
      attackSEParam.Pitch = 1.0f;
      attackSEParam.PlayLocation = player->GetActorLocation();
      attackSEParam.RepositoryAccessActor = player;
      attackSEParam.SEType = EARSoundEffectType::Attack;
      
      // Bind registry
      TSharedPtr<ARRanger::IObserverRegistry> attackRegistry = 
        MakeRegistry<ARRanger::FSoundEffectRegistry>(attackSEParam);
      attackRegistry->RegisterObserverProxy(attackObserverListNode);
    }

    // ジャンプ 
    TSharedPtr<FObserverListNode> jumpObserverListNode = FObserverListRootNode::MakeListNode();
    jumpObserverListNode->Initialize();

    {
      FARSoundEffectParameters jumpSEParam;
      jumpSEParam.Dimension = EARSoundDimension::Dimension3;
      jumpSEParam.Pitch = 1.0f;
      jumpSEParam.PlayLocation = player->GetActorLocation();
      jumpSEParam.RepositoryAccessActor = player;
      jumpSEParam.SEType = EARSoundEffectType::Jump;
      
      TSharedPtr<ARRanger::IObserverRegistry> jumpRegistry =
        MakeRegistry<ARRanger::FSoundEffectRegistry>(jumpSEParam);
      
      jumpRegistry->RegisterObserverProxy(jumpObserverListNode);
    }

    // ダッシュ
    TSharedPtr<FObserverListNode> dashObserverListNode = FObserverListRootNode::MakeListNode();
    dashObserverListNode->Initialize();

    {
      FARSoundEffectParameters dashSEParam;
      dashSEParam.Dimension = EARSoundDimension::Dimension3;
      dashSEParam.Pitch = 5.0f;
      dashSEParam.PlayLocation = player->GetActorLocation();
      dashSEParam.RepositoryAccessActor = player;
      dashSEParam.SEType = EARSoundEffectType::Dash;

      TSharedPtr<ARRanger::IObserverRegistry> dashRegistry =
        MakeRegistry<ARRanger::FSoundEffectRegistry>(dashSEParam);
      
      dashRegistry->RegisterObserverProxy(dashObserverListNode);
    }

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
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());

            FTimerHandle ClearTimerHandle;
            GetWorldTimerManager().SetTimer(ClearTimerHandle, this, &AInsekiGameMode::HandleGameClear, 3.0f, false);
		}
	}
}

void AInsekiGameMode::HandleGameClear()
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