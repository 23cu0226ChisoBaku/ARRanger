//*************************************************
// 引力必殺技の制御コンポーネント
//*************************************************

#include "PlayerComponents/AttractSpecialAttackComponent.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "SpecialAttackAttractActor.h"

#include "GameFramework/Character.h"

UAttractSpecialAttackComponent::UAttractSpecialAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UAttractSpecialAttackComponent::BeginPlay()
{
	Super::BeginPlay();	
}
void UAttractSpecialAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(m_IsAttractKick)
	{
		SpecialFinishKick(DeltaTime);
	}
	else if(m_IsLand)
	{
		m_ElapsedTime += DeltaTime;
		if(m_LandTime <= m_ElapsedTime)
		{
			ResetParameter();
		}
	}
}

/**
 * @brief 引力必殺技を始めた際の処理
 */
void UAttractSpecialAttackComponent::OnStartSpecialAttract()
{
	m_IsGenerateAttract = true;

	/*引き寄せるアクターを生成*/
	GenerateAttractActor();

	/*指定した時間後キック！*/
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]()
	{
		m_kickDirection = ( m_GenerateArractActor->GetActorLocation() - GetOwner()->GetActorLocation() ).GetSafeNormal();
		m_IsAttractKick = true;
	});

	GetWorld()->GetTimerManager().SetTimer(
		m_DelayTimerHandle,
		TimerDelegate,
		m_AttractTime,
		false
	);

}

/**
 * @brief 対象のオブジェクトを引き寄せる物体を生成する
 */
void UAttractSpecialAttackComponent::GenerateAttractActor()
{
	/*ライントレースの始点と終点*/
	FVector startLocation = GetOwner()->GetActorLocation();
	FVector generateDirection = FVector(GetPlayerCameraRotation().X, GetPlayerCameraRotation().Y, 0.0f).GetSafeNormal();
	FVector endLocation = startLocation + generateDirection * m_GenerateDistance;

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	params.bReturnPhysicalMaterial = false;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		hitResult,
		startLocation,
		endLocation,
		ECC_Visibility,
		params
	);

	/*引力アクターを生成する座標*/
	FVector generatLocation;
	if(bHit)
	{
		generatLocation = hitResult.Location - m_OffsetGenerateDistance;
	}
	else
	{
		generatLocation = endLocation;
	}

	FActorSpawnParameters SpawnParams;
	m_GenerateArractActor = GetWorld()->SpawnActor<AActor>(
		m_AttractActor,
		generatLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
} 

/**
 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(後からなくなる)
 */
FVector UAttractSpecialAttackComponent::GetPlayerCameraRotation()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
			{
				return CamMgr->GetCameraRotation().Vector();
			}
		}
	}
	return FVector::ZeroVector;
}

/**
 * @brief 引力必殺技に関するパラメータをリセットする関数
 */
void UAttractSpecialAttackComponent::ResetParameter()
{
	m_ElapsedTime = 0.0f;
	m_IsGenerateAttract = false;
	m_IsBackFlip = false;
	m_IsAttractKick = false;
	m_IsLand = false;
}

/**
 * @brief 対象のアクターを引き寄せている場所にキック!!
 * 
 * @param １フレームにかかる時間
 */
void UAttractSpecialAttackComponent::SpecialFinishKick(float deltaTime)
{
	/*指定時間が過ぎたらキックを終了する*/
	if(m_KickTime <= m_ElapsedTime)
	{
		m_CurrentKickSpeed -= m_KickBrakingForce;
		if(m_CurrentKickSpeed <= 0.0f)
		{
			m_IsAttractKick = false;
			m_IsLand = true;
			m_ElapsedTime = 0.0f;
		}
		return;
	}

	/*時間計測*/
	m_ElapsedTime += deltaTime;

	/*キック！*/
    m_CurrentKickSpeed = m_CustomCurveSpeed->GetFloatValue(m_ElapsedTime);
	FVector newLocation = GetOwner()->GetActorLocation() + m_kickDirection * m_CurrentKickSpeed;
	GetOwner()->SetActorLocation(newLocation);
}