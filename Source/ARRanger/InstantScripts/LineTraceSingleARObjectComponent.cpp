// ARObjectをラインとレースで取得するコンポーネント(即席スクリプト)

#include "InstantScripts/LineTraceSingleARObjectComponent.h"


void ULineTraceSingleARObjectComponent::PerformRaycast()
{
    // 開始位置（コンポーネントの親アクターの位置）
    FVector Start = GetOwner()->GetActorLocation();

    // 方向（親アクターの前方ベクトル）
    FVector ForwardVector = GetOwner()->GetActorForwardVector();
    FVector End = Start + (ForwardVector * LineTraceLength);

    // 衝突設定
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());  // レイがコンポーネントの親アクターを無視するように設定

    // 取得結果
    FHitResult HitResult;

    // レイキャストを実行
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        CollisionParams
    );

    if (bHit)
    {
        /*ライントレースで検知したオブジェクトを保持*/ 
        AActor* HitActor = HitResult.GetActor();

        /*ヒットしたオブジェクトがインターフェースを実装しているかをチェック*/ 
        if (HitActor && HitActor->Implements<IARObjectInterface>())
        {
            // インターフェースをキャスト
            IARObjectInterface* InterfaceActor = Cast<IARObjectInterface>(HitActor);
            if (InterfaceActor)
            {
                // インターフェース関数を呼び出す
                //InterfaceActor->SetNewARType();
            }
        }
    }
}

ULineTraceSingleARObjectComponent::ULineTraceSingleARObjectComponent()
    : LineTraceLength(1000.0f)
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

