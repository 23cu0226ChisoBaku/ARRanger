//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#include "InstantScripts/LineTraceSingleARObjectComponent.h"

#include "GameFramework/GameplayCameraComponent.h"

/*Start ULineTraceSingleARObjectComponent Lifecycle Functions*/
ULineTraceSingleARObjectComponent::ULineTraceSingleARObjectComponent()
    : LineTraceLength(1000.0f)
    , m_TargetMagnetizableActor(nullptr)
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
/*End ULineTraceSingleARObjectComponent Lifecycle Functions*/

/**
 * @brief コンポーネント所有者についているカメラコンポ―ネントを取得する関数
 * 
 * @param コンポーネント所有者についているカメラコンポ―ネント
 */
void ULineTraceSingleARObjectComponent::SetPlayerCameraComp(const UGameplayCameraComponent* playerCameraComp)
{
    if(playerCameraComp)
    { 
        m_PlayerCameraComp = const_cast<UGameplayCameraComponent*>(playerCameraComp);
    }
}

/*
* @brief ライントレースを行って付与できるオブジェクトを検知
*
* @param ライントレースを行うための始点と終点
*
* @return 検知した MagnetizableObject
*/
AActor* ULineTraceSingleARObjectComponent::TraceForMagnetizableObject(const FVector& Start, const FVector& End)
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
        if (HitActor->GetClass()->ImplementsInterface(UARMagnetizableInterface::StaticClass()))
        {
            // オブジェクト名をログに出力
            UE_LOG(LogTemp, Log, TEXT("Hit ARObject: %s"), *HitActor->GetName());
            return HitActor;
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
    
    return nullptr;
}

/** 
 * @brief BPでデリゲートを呼び出す
 */
void ULineTraceSingleARObjectComponent::ExecuteSetTargetMagnetizableObject()
{
    if(SetTargetMagnetizableObject.IsBound())
    {
        if(m_TargetMagnetizableActor)
        {
            SetTargetMagnetizableObject.Execute(m_TargetMagnetizableActor);
        }
    }
}
