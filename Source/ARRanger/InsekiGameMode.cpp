#include "InsekiGameMode.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ARRangerCharacter.h"

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
#include "BlinkingSystem/DetectorMagnetizableComponent.h"

#include "ARRangerGlobals.h"
#include "GameplayFramework/ARGameInstance.h"
#include "Enemy/Enemy_MiddleBoss.h"
#include "BattleEvent/BattleEventManager.h"
#include "BattleEvent/EnemySpawner.h"
#include "GameFramework/GameUserSettings.h"

// TODO May move initialize function to another file
#include "Physics/IARPhysicsSystemHost.h"

namespace Private
{
  struct FARGameUserSettingsData
  {
    double FrameLimit = 0.0;
  };
}

namespace
{
  constexpr int32 MAIN_PLAYER_INDEX = 0;
  constexpr float ACTION_FRAME_LIMIT = 60.0f;
  TArray<Private::FARGameUserSettingsData> gSettingsDataStack;
}


AInsekiGameMode::AInsekiGameMode()
  : bGameResultHandled{false}
  , GameResultTimerHandle{}
{
  ProcessorActorClass = AARPhysicsTickProcessorActor::StaticClass();
  DefaultPawnClass = AARRangerCharacter::StaticClass();
}


void AInsekiGameMode::BeginPlay()
{
	Super::BeginPlay();

  GameResultTimerHandle.Invalidate();
  // Register GameInstance OnReset
  if (UARGameInstance* ARGI = ::Cast<UARGameInstance>(GetGameInstance()))
  {
    ARGI->OnReset.AddUObject(this, &ThisClass::OnResetCommandSent);
  }

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
  InitializeEvents();
  InitializeOnMapEnemies();

  // 物理システム初期化
  ARRanger::Private::FARPhysicsCore::InitializeARPhysicsInWorldWithActorType(GetWorld(), ProcessorActorClass);

  // Register debug key
  ARRanger::Global::RegisterDebugKey();

  /*Event Manager デリゲートバインド*/
  RegisterBattleEventDelegate();

  SetGameUserSettings(gSettingsDataStack);
}

void AInsekiGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  ARRanger::Global::UnregisterDebugKey();

  // Remove GameInstance OnReset
  if (UARGameInstance* ARGI = ::Cast<UARGameInstance>(GetGameInstance()))
  {
    ARGI->OnReset.RemoveAll(this);
  }
  
  UnregisterBattleEventDelegate();
  UninitializeAliveEnemies();
  UninitializeEvents();

  UnsetGameUserSettings(gSettingsDataStack);
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

void AInsekiGameMode::OnEnemyDead(AActor* InEnemy)
{
	(void)EnemyCount--;

  if (BossPtr != nullptr)
  {
    if (InEnemy == BossPtr)
    {
      ProcessGameResult(GameClear);
    }
  }
}

/**
 * @brief バトルイベントが開始した際の処理
 */
void AInsekiGameMode::OnStartBattleEvent()
{
  ACharacter* playerChar = UGameplayStatics::GetPlayerCharacter(this, MAIN_PLAYER_INDEX);
  if (AARRangerCharacter* arPlayerChar = ::Cast<AARRangerCharacter>(playerChar))
  {
    // AnimInstance内の戦闘中フラグを上げる
    arPlayerChar->SetIsBattledInAnimInstance(true);
  }
}

/**
 * @brief バトルイベントが終了した際の処理
 */
void AInsekiGameMode::OnEndBattleEvent()
{
  ACharacter* playerChar = UGameplayStatics::GetPlayerCharacter(this, MAIN_PLAYER_INDEX);
  if (AARRangerCharacter* arPlayerChar = ::Cast<AARRangerCharacter>(playerChar))
  {
    // AnimInstance内の戦闘中フラグを下げる
    arPlayerChar->SetIsBattledInAnimInstance(false);
  }
}

void AInsekiGameMode::InitializeEvents()
{
  // Find boss
  AActor* boss = UGameplayStatics::GetActorOfClass(this, AEnemy_MiddleBoss::StaticClass());
  if (boss != nullptr)
  {
    BossPtr = boss;
  }

  bGameResultHandled = false;

  TArray<AActor*> allSpawners{};
  UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawner::StaticClass(), allSpawners);
  for (AActor* spawnerActor : allSpawners)
  {
    AEnemySpawner* spawner = ::Cast<AEnemySpawner>(spawnerActor);
    spawner->GetSpawnedActorDelegate.AddUObject(this, &ThisClass::OnEnemySpawned);
  }

  // Register Player Dead Event
  if (AARRangerCharacter* player = ::Cast<AARRangerCharacter>(UGameplayStatics::GetActorOfClass(this, AARRangerCharacter::StaticClass())))
  {
    player->OnPlayerDead.AddUniqueDynamic(this, &ThisClass::ARGameOver);
  }

}

void AInsekiGameMode::ProcessGameResult(EGameResultState ResultState)
{
  if (bGameResultHandled)
  {
    return;
  }

  // Set clear timer
  auto gameResultHandler = [this, ResultState]()
  {
    if (UARGameInstance* gameInst = this->GetGameInstance<UARGameInstance>())
    {
      switch (ResultState)
      {
        case GameClear:
        {
          gameInst->ProcessGameClear();
        }
        break;

        case GameOver:
        {
          gameInst->ProcessGameOver();
        }
        break;
      }
    }
  };

  GetWorldTimerManager().SetTimer(GameResultTimerHandle, gameResultHandler, 8.0f, false);
  bGameResultHandled = true;
}

void AInsekiGameMode::OnResetCommandSent()
{
  if (GameResultTimerHandle.IsValid())
  {
    GetWorldTimerManager().ClearTimer(GameResultTimerHandle);
    GameResultTimerHandle.Invalidate();
  }
}

void AInsekiGameMode::InitializeOnMapEnemies()
{
  TArray<AActor*> onMapEnemies{};
  UGameplayStatics::GetAllActorsOfClass(this, AEnemy_Zako::StaticClass(), onMapEnemies);
  for (AActor* foundActorPtr : onMapEnemies)
  {
    AEnemy_Zako* enemy = ::Cast<AEnemy_Zako>(foundActorPtr);
    enemy->OnEnemyDeadEnded.AddUniqueDynamic(this, &ThisClass::OnEnemyDead);
  }
}

void AInsekiGameMode::RegisterBattleEventDelegate()
{
  TArray<AActor*> eventManager{};
  UGameplayStatics::GetAllActorsOfClass(this, ABattleEventManager::StaticClass(), eventManager);
  if (eventManager.Num() > 0)
  {
    // Register delegate to first manager we found
    ABattleEventManager* manager = ::Cast<ABattleEventManager>(eventManager[0]);

    manager->OnAnyFieldBattleStart.AddUniqueDynamic(this, &AInsekiGameMode::OnStartBattleEvent);
    manager->OnAnyFieldBattleEnd.AddUniqueDynamic(this, &AInsekiGameMode::OnEndBattleEvent);
  }
  
}

void AInsekiGameMode::UnregisterBattleEventDelegate()
{
  TArray<AActor*> eventManager{};
  UGameplayStatics::GetAllActorsOfClass(this, ABattleEventManager::StaticClass(), eventManager);
  if (eventManager.Num() > 0)
  {
    // Register delegate to first manager we found
    ABattleEventManager* manager = ::Cast<ABattleEventManager>(eventManager[0]);

    manager->OnAnyFieldBattleStart.RemoveDynamic(this, &AInsekiGameMode::OnStartBattleEvent);
    manager->OnAnyFieldBattleEnd.RemoveDynamic(this, &AInsekiGameMode::OnEndBattleEvent);
  }
}

void AInsekiGameMode::UninitializeAliveEnemies()
{
  TArray<AActor*> onMapEnemies{};
  UGameplayStatics::GetAllActorsOfClass(this, AEnemy_Zako::StaticClass(), onMapEnemies);
  for (AActor* foundActorPtr : onMapEnemies)
  {
    AEnemy_Zako* enemy = ::Cast<AEnemy_Zako>(foundActorPtr);
    enemy->OnEnemyDeadEnded.RemoveDynamic(this, &ThisClass::OnEnemyDead);
  }
}

void AInsekiGameMode::OnEnemySpawned(AActor* InSpawnedEnemy)
{
  if (InSpawnedEnemy == nullptr)
  {
    return;
  }

  if (AEnemy_Zako* enemy = ::Cast<AEnemy_Zako>(InSpawnedEnemy))
  {
    ++EnemyCount;
    enemy->OnEnemyDeadEnded.AddUniqueDynamic(this, &ThisClass::OnEnemyDead);

    if (AEnemy_MiddleBoss* middleBoss = ::Cast<AEnemy_MiddleBoss>(enemy))
    {
      BossPtr = middleBoss;
    }
  }
}

void AInsekiGameMode::UninitializeEvents()
{
  TArray<AActor*> allSpawners{};
  UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawner::StaticClass(), allSpawners);
  for (AActor* spawnerActor : allSpawners)
  {
    AEnemySpawner* spawner = ::Cast<AEnemySpawner>(spawnerActor);
    spawner->GetSpawnedActorDelegate.RemoveAll(this);
  }
}

void AInsekiGameMode::ARGameOver()
{
  ProcessGameResult(GameOver);
}

void AInsekiGameMode::SetGameUserSettings(TArray<Private::FARGameUserSettingsData>& OutSettingsDataStack)
{
  if (UGameUserSettings* GUS = UGameUserSettings::GetGameUserSettings())
  {

    Private::FARGameUserSettingsData prevData{};
    prevData.FrameLimit = GUS->GetFrameRateLimit();

    // Set Frame rate
    GUS->SetFrameRateLimit(ACTION_FRAME_LIMIT);

    OutSettingsDataStack.Push(prevData);
  }
}

void AInsekiGameMode::UnsetGameUserSettings(TArray<Private::FARGameUserSettingsData>& OutSettingsDataStack)
{
  if (UGameUserSettings* GUS = UGameUserSettings::GetGameUserSettings())
  {
    Private::FARGameUserSettingsData prevData = OutSettingsDataStack.Pop();
    GUS->SetFrameRateLimit(prevData.FrameLimit);
  }
}