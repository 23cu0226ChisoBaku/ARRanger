
#include "Enemy/MiddleBossAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy/Enemy_Zako.h"

AMiddleBossAIController::AMiddleBossAIController()
{
  static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAsset(TEXT("/Game/Enemy/BB_MiddleEnemy.BB_MiddleEnemy"));
  static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/Enemy/BT_MiddleEnemy.BT_MiddleEnemy"));

  if (BBAsset.Succeeded()) {
      BlackboardAsset = BBAsset.Object;
  }

  if (BTAsset.Succeeded()) {
      BehaviorTreeAsset = BTAsset.Object;
  }

  SightConfig->SightRadius = 2000.0f;
  SightConfig->LoseSightRadius = 2500.0f;
  SightConfig->PeripheralVisionAngleDegrees = 120.0f;


}

void AMiddleBossAIController::BeginPlay()
{
  Super::BeginPlay();

  if (BlackboardAsset && BehaviorTreeAsset)
  {
    RunBehaviorTree(BehaviorTreeAsset);
  }
}

void AMiddleBossAIController::StopChasing()
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if (BB)
    {
      GetWorld()->GetTimerManager().SetTimer(
          LostSightTimerHandle,[BB, this]()
          {
            BB->ClearValue(TargetActorKey);
            BB->SetValueAsBool("IsPlayerDetected", false);
          },
          5.0f,
          false
      );
    }
}

void AMiddleBossAIController::BroadcastAlert(AActor* SeenActor)
{
  Super::BroadcastAlert(SeenActor);

  if (UBlackboardComponent* BB = GetBlackboardComponent())
  {
    BB->SetValueAsBool("IsEnraged", true);
  }
}