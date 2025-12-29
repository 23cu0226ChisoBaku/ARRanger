#include "AttractComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "CollisionShape.h" 
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WorldCollision.h" 
//#include "ForceableInterface.h"

UAttractComponent::UAttractComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UAttractComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAttractComponent::TriggerForce(const FVector& Origin, bool bRepulsive)
{
    // サーバー権限があれば直接適用、なければサーバーRPCを呼ぶ
    if (GetOwner()->HasAuthority())
    {
        ApplyRadialForceAt(Origin, bRepulsive);
    }
    else
    {
        Server_TriggerForce(Origin, bRepulsive);
    }
}

void UAttractComponent::Server_TriggerForce_Implementation(const FVector& Origin, bool bRepulsive)
{
    // サーバー側で実際に力を適用する（信頼できる処理）
    ApplyRadialForceAt(Origin, bRepulsive);
}

float UAttractComponent::ComputeFalloff(float Distance) const
{
    // 距離に応じた減衰を返す。ここはゲーム性に合わせて調整
    float t = FMath::Clamp(Distance / Radius, 0.f, 1.f);
    if (Falloff <= 0.0f)
    {
        return 1.0f;
    }

    // 簡易的な減衰関数。必要ならカーブやLookupTableに置き換え可能。
    return 1.0f / FMath::Pow(1.0f + t * (Falloff * 10.0f), Falloff);
}

void UAttractComponent::ApplyRadialForceAt(const FVector& Origin, bool bRepulsive)
{
    UWorld* World = GetWorld();
    if (!World) return;

    // オブジェクトタイプや無視リストを設定
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    // 必要に応じて追加

    TArray<AActor*> OutActors;
    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(GetOwner());

    // 範囲検出：Start と End を同じにして球状検出を行う
    const FVector Start = Origin;
    const FVector End = Origin;
    const float RadiusLocal = Radius;

    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        this,
        Start,
        RadiusLocal,
        ObjectTypes,
        nullptr,        // クラスフィルタ（nullptrで無視）
        IgnoreActors,
        OutActors
    );

    if (!bHit) return;

    for (AActor* Other : OutActors)
    {
        if (!Other) continue;

        // 物理コンポーネントを探す（ルートや子コンポーネントを検索）
        UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Other->GetRootComponent());
        if (!Prim)
        {
            // ルートに無ければコンポーネント列から探す
            TArray<UPrimitiveComponent*> PrimComps;
            Other->GetComponents<UPrimitiveComponent>(PrimComps);
            for (UPrimitiveComponent* C : PrimComps)
            {
                if (C && C->IsSimulatingPhysics())
                {
                    Prim = C;
                    break;
                }
            }
        }

        // 以降は既存ロジックと同様に距離・方向・力を計算して適用
        FVector ToOther = (Other->GetActorLocation() - Origin);
        float Dist = ToOther.Size();
        if (Dist <= KINDA_SMALL_NUMBER) Dist = 1.f;
        FVector Dir = ToOther / Dist;
        if (!bRepulsive) Dir *= -1.f;

        float FalloffScale = ComputeFalloff(Dist);
        float AppliedStrength = Strength * FalloffScale;

        if (Prim && Prim->IsSimulatingPhysics() && bAffectPhysics)
        {
            FVector Force = Dir * AppliedStrength;
            if (Mode == EForceMode::Impulse)
            {
                Prim->AddImpulse(Force, NAME_None, true);
            }
            else
            {
                Prim->AddForce(Force, NAME_None, true);
            }
        }
        else if (bAffectNonPhysics)
        {
            /*if (Other->GetClass()->ImplementsInterface(UForceableInterface::StaticClass()))
            {
                IForceableInterface::Execute_ApplyRadialForce(Other, Origin, Dir * AppliedStrength, bRepulsive);
            }
            else
            {
                APawn* Pawn = Cast<APawn>(Other);
                if (Pawn)
                {
                    ACharacter* Char = Cast<ACharacter>(Pawn);
                    if (Char)
                    {
                        FVector LaunchVel = Dir * (AppliedStrength / FMath::Max(1.f, Char->GetCharacterMovement()->Mass));
                        Char->LaunchCharacter(LaunchVel, true, true);
                    }
                }
            }*/
        }
    }
}

