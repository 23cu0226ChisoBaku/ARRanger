#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "WorldCollision.h"

#include "AttractComponent.generated.h"

// 力のかけ方を判断
UENUM(BlueprintType)
enum class EForceMode : uint8
{
    // 瞬間的な力(吹き飛ばしなど)
    Impulse,
    // 断続的な力(引き寄せなど)
    Force
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARRANGER_API UAttractComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttractComponent();

    // 力の影響範囲（半径）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force")
    float Radius = 1000.f;

    // 基本強度（距離減衰前のベース値）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force")
    float Strength = 2000.f;

    // 力の適用モード（Impulse or Force）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force")
    EForceMode Mode = EForceMode::Impulse;

    // 減衰の係数（1 = 線形、2 = 二乗など調整可能）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force")
    float Falloff = 1.0f;

    // 物理シミュレーション有効なコンポーネントに影響を与えるか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force")
    bool bAffectPhysics = true;

    // 物理非対応（キャラ等）にも影響を与えるか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force")
    bool bAffectNonPhysics = true;

    // Blueprintや他から呼べるトリガー関数
    // Origin: 力の中心位置、bRepulsive: true=斥力(外向き)、false=引力(内向き)
    UFUNCTION(BlueprintCallable, Category = "Force")
    void TriggerForce(const FVector& Origin, bool bRepulsive);

    // マルチプレイ対応：サーバー側で実行するRPC
    UFUNCTION(Server, Reliable)
    void Server_TriggerForce(const FVector& Origin, bool bRepulsive);

protected:
    virtual void BeginPlay() override;

private:
    // 実際に範囲内のオブジェクトを検出して力を適用する内部処理
    void ApplyRadialForceAt(const FVector& Origin, bool bRepulsive);

    // 距離に応じた減衰を計算するヘルパー
    float ComputeFalloff(float Distance) const;
};