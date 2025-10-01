//*************************************************
// 引力必殺技の制御コンポーネント
//*************************************************

#include "PlayerComponents/AttractSpecialAttackComponent.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "SpecialAttackAttractActor.h"
#include "HitStopHelper.h"
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
		if(m_LandTimeInterval <= m_ElapsedTime)
		{
			ResetParameter();
		}

		if(m_GenerateArractActor != nullptr)
		{
			m_GenerateArractActor->Destroy();
		}
	}
}
void UAttractSpecialAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
}

/**
 * @brief 引力必殺技を始めた際の処理
 */
void UAttractSpecialAttackComponent::StartSpecialAttract()
{
  m_IsGenerateAttract = true;

	/*キックする方向を取得*/
	m_kickDirection = FVector(GetPlayerCameraRotation().X, GetPlayerCameraRotation().Y, 0.0f).GetSafeNormal();

	/*引き寄せるアクターを生成*/
	GenerateAttractActor();

	/*プレイヤーの向きを修正*/
	const FRotator newFaceDir = GetPlayerCameraRotation().Rotation();
	GetOwner()->SetActorRotation(newFaceDir);
}

void UAttractSpecialAttackComponent::PerformSpecialAttract()
{
  if(m_GenerateArractActor!= nullptr)
  {
    if(ASpecialAttackAttractActor* attractActor = Cast<ASpecialAttackAttractActor>(m_GenerateArractActor))
    {
      m_InhaledActors = attractActor->GetDetectedActors();
    }

    m_IsGenerateAttract = true;
    m_IsAttractKick = true;
  }
}

/**
 * @brief 対象のオブジェクトを引き寄せる物体を生成する
 */
void UAttractSpecialAttackComponent::GenerateAttractActor()
{
	/*ライントレースの始点と終点*/
	FVector startLocation = GetOwner()->GetActorLocation();
	FVector endLocation = startLocation + m_kickDirection * m_GenerateDistance;

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
		generatLocation = hitResult.Location - m_GenerateDistanceOffset * m_kickDirection;
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
	if(m_KickTimeInterval <= m_ElapsedTime)
	{
		/*キックスピードを減速*/
		m_CurrentKickSpeed -= m_KickBrakingForce;
		FVector deltaLocation = m_kickDirection * FMath::Max(m_CurrentKickSpeed, 0.0f) * deltaTime;
    	GetOwner()->AddActorWorldOffset(deltaLocation, true);

		if(m_CurrentKickSpeed <= 0.0f)
		{
      OnSpecialAttractFinished.Broadcast();

			m_IsAttractKick = false;
			m_IsLand = true;
			m_ElapsedTime = 0.0f;
		}
		return;
	}

	/*時間計測*/
	m_ElapsedTime += deltaTime;

	/*ライントレースを行い建物などを検知したら進まない*/
	FVector startLocation = GetOwner()->GetActorLocation();
	FVector endLocation = startLocation + m_kickDirection * m_KickHitDetectionLength;

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(GetOwner());
	for(AActor* IgnoreActor : m_InhaledActors)
	{
		params.AddIgnoredActor(IgnoreActor);
	}
	params.bReturnPhysicalMaterial = false;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		hitResult,
		startLocation,
		endLocation,
		ECC_Visibility,
		params
	);

	/*キック！*/
	FVector newLocation;
	if(!bHit)
	{
		/*CurveFloatにそったスピードを計算*/
		float kickTimeIntervalNormalized = m_ElapsedTime / m_KickTimeInterval;
		float normalizedCurveValue = m_KickCurveSpeed->GetFloatValue(kickTimeIntervalNormalized);
		m_CurrentKickSpeed = m_KickMaxSpeed * normalizedCurveValue;
		FVector deltaLocation = m_kickDirection * m_CurrentKickSpeed;
		newLocation = GetOwner()->GetActorLocation() + deltaLocation;
	}
	/*障害物への埋め込み防止*/
	else
	{
		FVector addLocation = hitResult.Location - GetOwner()->GetActorLocation();
		newLocation = GetOwner()->GetActorLocation() + addLocation -m_KickPositionOffset * m_kickDirection;
	}
	GetOwner()->SetActorLocation(newLocation);
}