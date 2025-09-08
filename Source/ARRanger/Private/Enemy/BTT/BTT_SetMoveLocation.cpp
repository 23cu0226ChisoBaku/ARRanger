
#include "Enemy/BTT/BTT_SetMoveLocation.h"

EBTNodeResult::Type UBTT_SetMoveLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
    APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();

    if (!Target || !ControlledPawn) return EBTNodeResult::Failed;

    float PreferredDistance = BB->GetValueAsFloat("PreferredDistance");

    FVector ToTarget = ControlledPawn->GetActorLocation() - Target->GetActorLocation();
    ToTarget.Normalize();

    FVector DesiredLocation = Target->GetActorLocation() + ToTarget * PreferredDistance;

    BB->SetValueAsVector("MoveLocation", DesiredLocation);

    return EBTNodeResult::Succeeded;
}
