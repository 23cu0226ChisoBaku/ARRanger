//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#include "InstantScripts/LineTraceSingleARObjectComponent.h"
#include "IARMagnetizableInterface.h"

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

    AssignTargetMagnetizableObject();
}
/*End ULineTraceSingleARObjectComponent Lifecycle Functions*/

/**
 * @brief コンポーネント所有者についているカメラコンポ―ネントを取得する関数
 * 
 * @param コンポーネント所有者についているカメラコンポ―ネント
 */
void ULineTraceSingleARObjectComponent::SetPlayerCameraComp(const UGameplayCameraComponent* playerCameraComp)
{
    if(playerCameraComp != nullptr)
    { 
        m_PlayerCameraComp = const_cast<UGameplayCameraComponent*>(playerCameraComp);
    }
}

/**
 *  @brief 引力斥力を付与する対象のオブジェクトを他クラスに割り当てる処理
 */
void ULineTraceSingleARObjectComponent::AssignTargetMagnetizableObject()
{ 
    // 変数宣言
    AActor* currentTargetMagnetizableObj;   		 
    FVector lineTraceStart;                          
    FVector lineTraceEnd;                           

    // カメラが有効でなければリターン
    if (!m_PlayerCameraComp || !m_PlayerCameraComp->GetEvaluationContext().IsValid())  { return; }

    // ライントレース用の座標を取得
    lineTraceStart = GetOwner()->GetActorLocation();                                                                                                   // 始点
    lineTraceEnd = lineTraceStart +  GetPlayerCameraRotation() * LineTraceLength;   // 終点

    // ライントレースした結果を保持
    currentTargetMagnetizableObj = TraceForMagnetizableObject(lineTraceStart,lineTraceEnd);
    
    // 検知しているオブジェクトに変化があるかチェック
    if(currentTargetMagnetizableObj != m_TargetMagnetizableActor)
    {
        // 切り替わる前の対象オブジェクトに対する処理
        if(m_TargetMagnetizableActor != nullptr)
        {
            // IARObjectInterface を実装しているかチェック
            if (m_TargetMagnetizableActor->GetClass()->ImplementsInterface(UARMagnetizableInterface::StaticClass()))
            {
                IARMagnetizableInterface* interfacePtr = Cast<IARMagnetizableInterface>(m_TargetMagnetizableActor);
                if (interfacePtr)
                {
                    // 点滅処理を行うオブジェクトから除外
                    UnsetTargetMagnetizableObject.ExecuteIfBound(m_TargetMagnetizableActor);    
                }
            }       
        }

        // 切り替わった後の対象オブジェクトに対する処理
        if (currentTargetMagnetizableObj != nullptr)
        {
            // IARObjectInterface を実装しているかチェック
            if (currentTargetMagnetizableObj->GetClass()->ImplementsInterface(UARMagnetizableInterface::StaticClass()))
            {
                IARMagnetizableInterface* interfacePtr = Cast<IARMagnetizableInterface>(currentTargetMagnetizableObj);
                if (interfacePtr)
                {
                    // 点滅処理を行うオブジェクトとして登録
                    SetTargetMagnetizableObject.ExecuteIfBound(currentTargetMagnetizableObj);
                    m_TargetMagnetizableActor = currentTargetMagnetizableObj;
                }
            }
            else
            {
                m_TargetMagnetizableActor = nullptr;
            }
        } 
        else
        {
            m_TargetMagnetizableActor = nullptr;
        }
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

        // オブジェクト名をログに出力
        return HitActor;
                
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

/**
 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(後からなくなる)
 */
FVector ULineTraceSingleARObjectComponent::GetPlayerCameraRotation()
{
    APawn* Pawn = Cast<APawn>(GetOwner());
    if(Pawn != nullptr)
    {
        AController* Controller = Pawn->GetController();
        if(Controller != nullptr)
        {
            APlayerController* PlayerController = Cast<APlayerController>(Controller);
            if(PlayerController != nullptr)
            {
                APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

                if(CameraManager != nullptr)
                {
                    return CameraManager->GetCameraRotation().Vector();
                }
            }
        }
    }

    return FVector::ZeroVector;
}
