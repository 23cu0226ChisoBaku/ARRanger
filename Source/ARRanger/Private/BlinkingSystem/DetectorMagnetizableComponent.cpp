//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#include "Public/BlinkingSystem/DetectorMagnetizableComponent.h"
#include "IARMagnetizableInterface.h"
#include "ARObject/MagnetizableActor.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"


/*=============== Start UDetectorMagnetizableComponent Lifecycle Functions ===============*/
UDetectorMagnetizableComponent::UDetectorMagnetizableComponent() 
	: m_TargetMagnetizableActor(nullptr)
	, m_PlayerCameraComponent(nullptr)
	, m_DetectionRadius(500.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UDetectorMagnetizableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	m_OwnerActor = GetOwner();
}
void UDetectorMagnetizableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// 半径範囲の検出
	UpdateDetectedActors();
	// ライントレースによる単一ターゲット検出
	AssignTargetMagnetizableObject();
}
/* =============== End UDetectorMagnetizableComponent Lifecycle Functions ===============*/

/**
 * @brief コンポーネント所有者についているカメラコンポ―ネントを取得する関数
 * 
 * @param コンポーネント所有者についているカメラコンポ―ネント
 */
void UDetectorMagnetizableComponent::SetPlayerCameraComponent(const UGameplayCameraComponent* playerCameraComp)
{
	if (playerCameraComp)
	{
		m_PlayerCameraComponent = const_cast<UGameplayCameraComponent*>(playerCameraComp);
	}
}

/**
 * @brief Playerを中心に指定した半径の磁性を持ったオブジェクトを検知
 */
void UDetectorMagnetizableComponent::UpdateDetectedActors()
{
	if (m_OwnerActor == nullptr) { return; }

	TArray<AActor*> overlappedActors;
	TArray<AActor*> detectedActors;

	// プレイヤーを中心とした球体範囲内のアクターを取得
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		m_OwnerActor->GetActorLocation(),			/*中心座標*/
		m_DetectionRadius,							/*検知範囲*/
		m_ObjectTypes,								/*検知するオブジェクトタイプ(UE側で指定)*/
		nullptr,									/*すべてのクラスを検知するクラス*/
		{ m_OwnerActor },							/*無視するアクター*/
		overlappedActors							/*検知したアクターの格納場所*/
	);

	// IARMagnetizableInterface 実装しているアクターを抽出
	for (AActor* Actor : overlappedActors)
	{
		if (Actor == nullptr) { continue; }

		// クラスフィルタに合うか判定
		for (TSubclassOf<AActor> AllowedClass : m_DetectionClassFilter)
		{
			if (Actor->IsA(AllowedClass))
			{
				if (!detectedActors.Contains(Actor))
				{
					detectedActors.Add(Actor);
				}
			}
		}
	}

	// 検知したものが変わった場合処理
	if (detectedActors != m_DetectedMagnetizableActors)
	{
		// 新しく検知したオブジェクトへの処理
		for (AActor* Actor : detectedActors)
		{
			if (!m_DetectedMagnetizableActors.Contains(Actor))
			{
				m_DetectedMagnetizableActors.Add(Actor);
				SetActorOnOutline.ExecuteIfBound(Actor);
			}
		}

		TArray<AActor*> RemoveActors;
		// 検知しなくなったオブジェクトへの処理
		for (AActor* Actor : m_DetectedMagnetizableActors)
		{
			if (!detectedActors.Contains(Actor))
			{
				RemoveActors.Add(Actor);
				UnsetActorOnOutline.ExecuteIfBound(Actor);
			}
		}
		for (AActor* Actor : RemoveActors)
		{
			m_DetectedMagnetizableActors.Remove(Actor);
		}
	}
}

/**
 *  @brief 引力斥力を付与する対象のオブジェクトを他クラスに渡す処理
 */
void UDetectorMagnetizableComponent::AssignTargetMagnetizableObject()
{
	// 変数宣言
    AActor* currentTarget;
    FVector startPoint;
    FVector endPoint;

	// カメラが有効でなければリターン
    if (m_PlayerCameraComponent == nullptr || !m_PlayerCameraComponent->GetEvaluationContext().IsValid())  { return; }

    // 始点と終点を計算
    startPoint = m_OwnerActor->GetActorLocation();								  /*プレイヤーの座標*/
    endPoint = startPoint + GetPlayerCameraRotation() * m_DetectionRadius;	  	  /*付与できる範囲*/

	// ライントレースで取得
	currentTarget = TraceForMagnetizableObject(startPoint, endPoint);

	// 対象が変更された場合
	if (currentTarget != m_TargetMagnetizableActor)
	{
		// 前の対象を解除
		if (m_TargetMagnetizableActor != nullptr && m_TargetMagnetizableActor->GetClass()->ImplementsInterface(UARMagnetizableInterface::StaticClass()))
		{
			UnsetMagnetizableObjectAtCursor.ExecuteIfBound(m_TargetMagnetizableActor);
		}

		// 新しい対象を設定
		if (currentTarget != nullptr && currentTarget->GetClass()->ImplementsInterface(UARMagnetizableInterface::StaticClass()))
		{	
			SetMagnetizableObjectAtCursor.ExecuteIfBound(currentTarget);
			m_TargetMagnetizableActor = currentTarget;
		}
		else
		{
			m_TargetMagnetizableActor = nullptr;
		}
	}
}

/**
 *  @brief プレイヤーの状態(斥力・引力)をカーソルのあっているオブジェクトに付与する
 */
void UDetectorMagnetizableComponent::ApplyMagnetism(AActor* targetActor)
{
	/*付与ってボタンの仕様があった時の処理*/
	// if(m_TargetMagnetizableActor == nullptr){return;}

	// if (AMagnetizableActor* MagnetActor = Cast<AMagnetizableActor>(m_TargetMagnetizableActor))
	// {
	// 	if(MagnetActor->CanSetMagnetismType())
	// 	{
	// 		MagnetActor->ElapsedBlinkTime = 0.0f;
	// 		SetActorOnBlinkingOutline.ExecuteIfBound(m_TargetMagnetizableActor);
	// 	}
	// }

	if(m_OwnerActor == nullptr)
	{
		return;
	}

	IARMagnetizableInterface* playerCharacter = Cast<IARMagnetizableInterface>(m_OwnerActor);
	if (playerCharacter == nullptr)
	{
		return;
	}

	if (AMagnetizableActor* magnetActor = Cast<AMagnetizableActor>(targetActor))
	{
		magnetActor->SetType(playerCharacter->GetMagnetismType());
		magnetActor->ElapsedBlinkTime = 0.0f;
		SetActorOnOutline.ExecuteIfBound(targetActor);
	}
}

/*
* @brief ライントレースを行って付与できるオブジェクトを検知
*
* @param ライントレースを行うための始点と終点
*
* @return 検知した MagnetizableObject
*/
AActor* UDetectorMagnetizableComponent::TraceForMagnetizableObject(const FVector& startpoint, const FVector& endpoint)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(m_OwnerActor);
	Params.bReturnPhysicalMaterial = false;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		startpoint,
		endpoint,
		ECC_Visibility,
		Params
	);

	if (bHit && HitResult.GetActor() != nullptr)
	{
		return HitResult.GetActor();
	}

	return nullptr;
}

/**
 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(後からなくなる)
 */
FVector UDetectorMagnetizableComponent::GetPlayerCameraRotation()
{
	APawn* Pawn = Cast<APawn>(m_OwnerActor);
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