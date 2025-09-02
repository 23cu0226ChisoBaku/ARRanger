//*************************************************
// 引力必殺技の対象オブジェクトを吸引するアクター
//*************************************************

#include "SpecialAttackAttractActor.h"

ASpecialAttackAttractActor::ASpecialAttackAttractActor()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ASpecialAttackAttractActor::BeginPlay()
{
	Super::BeginPlay();
}
void ASpecialAttackAttractActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> overlappedActors;
	TArray<AActor*> detectedActors;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); 

	/*プレイヤーを中心とした球体範囲内のアクターを取得*/ 
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),			/*中心座標*/
		m_DetectionRadius,			/*検知範囲*/
		m_ObjectTypes,				/*すべてのオブジェクトタイプを検知*/
		nullptr,					/*すべてのクラスを検知*/
		ActorsToIgnore,				/*自分自身は無視*/
		overlappedActors			/*検知したアクターの格納場所*/
	);

	/*IARMagnetizableInterface 実装しているアクターを抽出*/ 
	for (AActor* actor : overlappedActors)
	{
		if (actor == nullptr) { continue; }

		/*クラスフィルタに合うか判定*/ 
		for (TSubclassOf<AActor> allowedClass : m_AttractionClassFilter)
		{
			if (actor->IsA(allowedClass))
			{
				if (!detectedActors.Contains(actor))
				{
					detectedActors.Add(actor);
				}
			}
		}
	}

	/*detectedActors に含まれるすべてのアクターに対して処理を実行*/ 
    for (AActor* actor : detectedActors)
    {
        if (actor)
        {
			/*目的地に引き寄せられる*/
			FVector currentLocation = actor->GetActorLocation();
			FVector movedirection = (GetActorLocation() - currentLocation).GetSafeNormal();
			FVector newLocation = currentLocation + movedirection * m_AttractSpeed * DeltaTime;        
			actor->SetActorLocation(newLocation);

			#if m_IsMoveRotate  
				/*目的地を中心に周りをまわる*/
				FQuat rotation = FQuat(FVector(0.0f, 0.0f, 1.0f), FMath::DegreesToRadians(m_RotationSpeed * DeltaTime));
				FVector rotatedDirection = rotation.RotateVector(movedirection);
				FVector finalLocation = GetActorLocation() + rotatedDirection * m_DetectionRadius;
				Actor->SetActorLocation(finalLocation);

				/*アクターの向きを移動方向に向かせる*/
				FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(currentLocation, finalLocation);
				actor->SetActorRotation(newRotation);
			#endif
        }
    }

#if true
	DrawDebugSphere
	(
		GetWorld(),
		GetActorLocation(),     /*中心*/
		m_DetectionRadius,      /*半径*/
		16,                     /*セグメント数(丸さ)*/
		FColor::Blue,           /*色*/
		false,                  /*永続表示するか*/
		-1.0f                   /*表示時間(秒)*/
	);
#endif
}