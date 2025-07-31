// ARObjectをラインとレースで取得するコンポーネント(即席スクリプト)

#include "InstantScripts/LineTraceSingleARObjectComponent.h"

/*
* ライントレースを行い、引力斥力を付与できるオブジェクトを取得する関数
*/
void ULineTraceSingleARObjectComponent::TraceForARObject(const FVector& Start, const FVector& End)
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
* アウトラインの処理を呼び出すデリゲート関数
*/
void ULineTraceSingleARObjectComponent::OnBlinkingOutline(AActor* magnetizableObject)
{
    if (OnOutLine.IsBound())
    {
        OnOutLine.Execute(magnetizableObject);
        UE_LOG(LogTemp, Log, TEXT("OnOutLine invoke."));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("OnOutLine was not bound."));
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
