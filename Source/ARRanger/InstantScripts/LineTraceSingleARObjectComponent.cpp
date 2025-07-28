// ARObjectをラインとレースで取得するコンポーネント(即席スクリプト)

#include "InstantScripts/LineTraceSingleARObjectComponent.h"

/*
* ライントレースを行い、引力斥力を付与できるオブジェクトを取得する関数
*/
AActor* ULineTraceSingleARObjectComponent::TraceForARObject(const FVector& Start, const FVector& End, UWorld* World)
{
    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner()); 
    Params.bReturnPhysicalMaterial = false;

    bool bHit = World->LineTraceSingleByChannel(
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
            return HitActor;
        }
        else
        {
            // 取得したオブジェクトがInterfaceをもっていなかった
            UE_LOG(LogTemp, Warning, TEXT("Hit actor does not implement IARObjectInterface: %s"), *HitActor->GetName());
        }
    }
    else
    {
        // オブジェクトを検知できなかった
        UE_LOG(LogTemp, Warning, TEXT("No actor hit during line trace."));
    }

    return nullptr;
}

/*
* ULineTraceSingleARObjectComponent Lifecycle Functions
*/
ULineTraceSingleARObjectComponent::ULineTraceSingleARObjectComponent()
    : LineTraceLength(1000.0f)
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
