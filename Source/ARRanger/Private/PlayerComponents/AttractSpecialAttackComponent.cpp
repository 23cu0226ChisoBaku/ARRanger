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
 * @brief 対象のオブジェクトを引き寄せる物体を生成する
 */
void UAttractSpecialAttackComponent::GeneratAttractActor()
{
	/*ライントレースの始点と終点*/
	FVector startLocation = GetOwner()->GetActorLocation();
	FVector endLocation = startLocation + GetPlayerCameraRotation() * m_GeneratDistance;

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
	FVector generatLocation = hitResult.Location - m_OffsetGeneratDistance;

	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<ASpecialAttackAttractActor>(
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
