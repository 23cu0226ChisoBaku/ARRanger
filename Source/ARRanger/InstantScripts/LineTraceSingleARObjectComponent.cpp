//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#include "InstantScripts/LineTraceSingleARObjectComponent.h"

/*
* @brief ライントレースを行って付与できるオブジェクトを検知
*
* @param ライントレースを行うための始点と終点
*/
void ULineTraceSingleARObjectComponent::TraceForMagnetizableObject(const FVector& Start, const FVector& End)
{
    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner()); 
    Params.bReturnPhysicalMaterial = false;

    bool bHit = GetOwner()->GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility, 
        Params
    );

    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();

        // IARObjectInterface を実装しているかチェック
        if (HitActor->GetClass()->ImplementsInterface(UARObjectInterface::StaticClass()))
        {
            // オブジェクト名をログに出力
            UE_LOG(LogTemp, Log, TEXT("Hit ARObject: %s"), *HitActor->GetName());
            _pTargetMagnetizableActor = HitActor;
        }
        else
        {
            // 取得したオブジェクトがInterfaceをもっていなかった
            //UE_LOG(LogTemp, Warning, TEXT("Hit actor does not implement IARObjectInterface: %s"), *HitActor->GetName());
        }
    }
    else
    {
        // オブジェクトを検知できなかった
        //UE_LOG(LogTemp, Warning, TEXT("No actor hit during line trace."));
    }
}

/*
* Start ULineTraceSingleARObjectComponent Lifecycle Functions
*/
ULineTraceSingleARObjectComponent::ULineTraceSingleARObjectComponent()
    : LineTraceLength(1000.0f)
    , _pTargetMagnetizableActor(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULineTraceSingleARObjectComponent::BeginPlay()
{
	Super::BeginPlay();	
}

void ULineTraceSingleARObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
/*
* End ULineTraceSingleARObjectComponent Lifecycle Functions
*/
