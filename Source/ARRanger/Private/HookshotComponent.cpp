//*************************************************
// 引力フックショット(仮)の挙動を行うコンポーネント
//*************************************************

#include "HookshotComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UHookshotComponent::UHookshotComponent()
    : m_TargetActor(nullptr),
      m_CurrentHookshotSpeed(0.0f),
      m_ElapsedTime(0.0f),
      m_CanHookshot(true), 
      m_IsHookshotAction(false)
{
	PrimaryComponentTick.bCanEverTick = true;
    
    /*TMap にラムダを登録*/
    SetupSpeedCurveFunctions();
}
void UHookshotComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UHookshotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (m_IsHookshotAction)
    {
        HookshotAction(DeltaTime);
    }    
}

/**
 * @brief BPから呼ぶフックショット開始関数
 * 
 * @param 移動先ターゲットアクター
 */
void UHookshotComponent::StartHookshot(AActor* targetActor)
{
    if (!m_CanHookshot || targetActor == nullptr)
    {
        return;
    }

    /*プレイヤーにかかっている加速度をリセット*/
    if (ACharacter* character = Cast<ACharacter>(GetOwner()))
    {
        if (UCharacterMovementComponent* moveComp = character->GetCharacterMovement())
        {
            moveComp->StopMovementImmediately(); 
            moveComp->GravityScale = 0.f;  
            character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);     
        }
    }

    /*引力フックショットに必要なパラメータを設定*/
    m_TargetActor = targetActor;
    m_IsHookshotAction = true;
    m_CurrentHookshotSpeed = m_HookshotMinSpeed;
    m_ElapsedTime = 0.0f;
}

/**
 * @brief フックショットの処理
 */
void UHookshotComponent::HookshotAction(float deltaTime)
{
    if (!m_CanHookshot || m_TargetActor == nullptr)
    {
        StopHookshot();
        return;
    }

    ACharacter* character = Cast<ACharacter>(GetOwner());
    if (character == nullptr)
    {
        StopHookshot();
        return;
    } 

    /*重力をゼロにする*/
    character->GetCharacterMovement()->GravityScale = 0.f;

    /*フックショットのスピードを増加*/
    IncreaseHookshotSpeed(deltaTime);

    /*オーナーをターゲット方向に移動*/
    FVector ownerLocation = GetOwner()->GetActorLocation();
    FVector targetLocation = m_TargetActor->GetActorLocation();
    FVector direction = CalculationDirection(ownerLocation, targetLocation);

    FVector newLocation = ownerLocation + direction * m_CurrentHookshotSpeed * deltaTime;
    GetOwner()->SetActorLocation(newLocation);

    /*BoxCollisionが何かしらに接触した場合止める*/
    if(HitCheckOnHookshot(ownerLocation,direction))
    {
        StopHookshot();
        return;
    }

    /*デバッグ用*/
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,                                                         /*Key (-1 = 自動割り当て)*/
            1.0f,                                                       /*表示時間(秒)*/
            FColor::Green,                                              /*色*/ 
            FString::Printf(TEXT("ElapsedTime: %.2f"), m_ElapsedTime)   /*表示内容*/ 
        );
    }
}

/**
 * @brief 始点から終点に対しての方向ベクトルを計算
 * 
 * @param 始点,終点
 * 
 * @return 正規化した方向ベクトル
 */
FVector UHookshotComponent::CalculationDirection(FVector StartPos, FVector EndPos)
{
	return (EndPos - StartPos).GetSafeNormal();
}

/**
 * @brief フックショットのスピードを徐々に上げる関数
 * 
 * @param 1フレームにかかる時間
 */
void UHookshotComponent::IncreaseHookshotSpeed(float deltaTime)
{
    /*TMap で登録済みのラムダ関数を呼ぶ*/
    if (SpeedCurveFunctions.Contains(m_SpeedCurve))
    {
        SpeedCurveFunctions[m_SpeedCurve](deltaTime);
    }

    /*上限チェック（安全対策)*/
    m_CurrentHookshotSpeed = FMath::Min(m_CurrentHookshotSpeed, m_HookshotMaxSpeed);
}

/**
 * @brief フックショットの移動と障害物判定を行う
 * 
 * @param  コンポーネント所有者の座標 ,進行方向ベクトル(正規化済み)
 * 
 * @return フックショットをやめるかどうか
 */
bool UHookshotComponent::HitCheckOnHookshot(const FVector& ownerLocation ,const FVector& direction)
{
    /*開始位置にオフセットを加える*/
    FVector startLocation = ownerLocation + m_HookshotBoxTraceOffset;
    FVector endLocation = startLocation + direction * m_HookshotBoxTraceDistance;

    /*BoxTrace のサイズ*/
    FCollisionShape BoxShape = FCollisionShape::MakeBox(m_HookshotBoxExtent);
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    /*BoxTraceを進行方向に飛ばす*/
    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        startLocation,
        endLocation,
        FQuat::Identity,
        ECC_Visibility,
        BoxShape,
        Params
    );

#if false
    DrawDebugBox(GetWorld(), startLocation, m_HookshotBoxExtent, FQuat::Identity, FColor::Green, false, 1.f);
    DrawDebugBox(GetWorld(), endLocation, m_HookshotBoxExtent, FQuat::Identity, FColor::Red, false, 1.f);
    DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Blue, false, 1.f, 0, 2.f);
#endif

    /*ヒットしたらフックショットを停止*/
    return bHit;
}

/**
 * @brief フックショットをやめる際の処理
 */
void UHookshotComponent::StopHookshot()
{
    /*重力を元に戻す*/
    if (ACharacter* character = Cast<ACharacter>(GetOwner()))
    {
        character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        character->GetCharacterMovement()->GravityScale = 1.7f;
    }

    m_IsHookshotAction = false;
    m_CurrentHookshotSpeed = 0.f;
    m_ElapsedTime = 0.f;
}

/** 
 * @brief TMap にスピード曲線用のラムダを登録する 
*/
void UHookshotComponent::SetupSpeedCurveFunctions()
{
    /*Linear*/
    SpeedCurveFunctions.Add(EHookshotSpeedCurve::Linear, [this](float DeltaTime) 
    {
        m_CurrentHookshotSpeed += m_HookshotSpeedIncreaseValue * DeltaTime;
        m_CurrentHookshotSpeed = FMath::Min(m_CurrentHookshotSpeed, m_HookshotMaxSpeed);
    });

    /*Exponential*/
    SpeedCurveFunctions.Add(EHookshotSpeedCurve::Exponential, [this](float DeltaTime) 
    {
        m_CurrentHookshotSpeed = FMath::FInterpTo(m_CurrentHookshotSpeed, m_HookshotMaxSpeed, DeltaTime, m_HookshotSpeedIncreaseValue);
    });

    /*Logarithmic*/
    SpeedCurveFunctions.Add(EHookshotSpeedCurve::Logarithmic, [this](float DeltaTime) 
    {
        m_ElapsedTime += DeltaTime;
        m_CurrentHookshotSpeed = m_HookshotMaxSpeed * FMath::Clamp(FMath::Loge(1.f + m_HookshotSpeedIncreaseValue * m_ElapsedTime), 0.f, 1.f);
    });
    
    /*CurveAsset*/
    SpeedCurveFunctions.Add(EHookshotSpeedCurve::CurveAsset, [this](float DeltaTime) 
    {
        if(m_CustomCurveSpeed != nullptr)
        {
            m_ElapsedTime = (m_ElapsedTime + DeltaTime);
            m_CurrentHookshotSpeed = m_CustomCurveSpeed->GetFloatValue(m_ElapsedTime) * 100.0f;
        }
    });
}