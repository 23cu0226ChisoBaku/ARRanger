//*************************************************
// 引力必殺技の制御コンポーネント
//*************************************************

#include "PlayerComponents/AttractSpecialAttackComponent.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "SpecialAttackAttractActor.h"

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
}

/**
 * @brief 引力必殺技を始めた際の処理
 */
void UAttractSpecialAttackComponent::OnStartSpecialAttract()
{
	m_IsAttractSpecialAttack = true;

	/*引き寄せるアクターを生成*/
	GenerateAttractActor();

	/*指定した時間後キック！*/
    GetWorld()->GetTimerManager().SetTimer(
        m_DelayTimerHandle,
        this,
        &UAttractSpecialAttackComponent::SpecialFinishKick,
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
	FVector generateDirection = FVector(0.0f ,0.0f, GetPlayerCameraRotation().GetSafeNormal().Z);
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
	FVector generatLocation = hitResult.Location - m_OffsetGenerateDistance;

	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<>(
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
 * @brief 対象のアクターを引き寄せている場所にキック!!
 */
void UAttractSpecialAttackComponent::SpecialFinishKick()
{
	FVector kickdirection = ( m_AttractActor->GetActorLocation() - GetOwner()->GetActorLocation() ).GetSafeNormal();

}