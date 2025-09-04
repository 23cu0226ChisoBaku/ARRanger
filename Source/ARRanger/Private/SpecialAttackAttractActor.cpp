//*************************************************
// 引力必殺技の対象オブジェクトを吸引するアクター
//*************************************************

#include "SpecialAttackAttractActor.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SpecialAttackAttractActor)

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

	static TArray<AActor*> overlappedActors{};
	static TArray<AActor*> detectedActors{};
	overlappedActors.Reset();
	detectedActors.Reset();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); 

	/*プレイヤーを中心とした球体範囲内のアクターを取得*/ 
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),			/*中心座標*/
		m_DetectionRadius,			/*検知範囲*/
		m_ObjectTypes.Array(),		/*すべてのオブジェクトタイプを検知*/
		nullptr,					/*すべてのクラスを検知*/
		ActorsToIgnore,				/*自分自身は無視*/
		overlappedActors			/*検知したアクターの格納場所*/
	);

	/*IARMagnetizableInterface 実装しているアクターを抽出*/ 
	for (AActor* actor : overlappedActors)
	{
		/*クラスフィルタに合うか判定*/ 
		for (TSubclassOf<AActor> allowedClass : m_AttractionClassFilter)
		{
			if (actor->IsA(allowedClass))
			{
				detectedActors.AddUnique(actor);			
			}
		}
	}


	/*detectedActors に含まれるすべてのアクターに対して処理を実行*/ 
    for (AActor* actor : detectedActors)
    {
		// /*一時物理挙動を解除する*/
		//UPrimitiveComponent* actorMesComponent = actor->FindComponentByClass<UPrimitiveComponent>();
		// if(actorMesComponent != nullptr)
		// {
		// 	if(actorMesComponent->IsSimulatingPhysics())
		// 	{
		// 		//actorMesComponent->SetSimulatePhysics(false);
		// 	}
		// }

		/*目的地に引き寄せられる*/
		const FVector currentLocation = actor->GetActorLocation();
		const FVector movedirection = (GetActorLocation() - currentLocation).GetSafeNormal();
		FVector newLocation = currentLocation + movedirection * m_AttractSpeed * DeltaTime;
		
		/*目的地を中心に周りをまわる*/
		const FVector rotationAxis = GetActorUpVector().Cross(-movedirection);
		float Theta = 1000.f * DeltaTime;
		FVector v2 = -rotationAxis; 
		FVector verticalV = -movedirection - v2;
		FVector w = rotationAxis * -movedirection;
		FVector DashVerticalV  = FMath::Cos(Theta) * verticalV + FMath::Sin(Theta) * w;
		FVector DashV = DashVerticalV + v2;
		newLocation +=  DashV * m_RotationSpeed;

		actor->SetActorLocation(newLocation, true);


#if WITH_EDITOR
		if(GEngine)
		{
			/*デバッグ用*/				
			GEngine->AddOnScreenDebugMessage(
				-1,                                                         /*Key (-1 = 自動割り当て)*/
				10.0f,                                                      /*表示時間(秒)*/
				FColor::Green,                                              /*色*/ 
				FString::Printf(TEXT("DashV: [%s]"), *DashV.ToString())     /*表示内容*/ 
			);
		}
#endif
	}
    
#if WITH_EDITOR
	::DrawDebugSphere
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