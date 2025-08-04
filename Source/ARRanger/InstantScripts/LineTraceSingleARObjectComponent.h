//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/IARObjectInterface.h"
#include "Interface/IARTypeInterface.h"
#include "LineTraceSingleARObjectComponent.generated.h"

// デリゲート関数宣言
DECLARE_DELEGATE_OneParam(FOnOutlineDelegate, AActor*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API ULineTraceSingleARObjectComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ULineTraceSingleARObjectComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ライントレース用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineTrace")
	float LineTraceLength;

	/*
	* @brief デリゲート関数を登録する用
	*/
	FOnOutlineDelegate SetTargetMagnetizableObject;

	/*
	* @brief ライントレースを行って付与できるオブジェクトを検知
	*
	* @param ライントレースを行うための始点と終点
	*/
	UFUNCTION(BlueprintCallable, Category = "LineTrace")
	void TraceForMagnetizableObject(const FVector& Start, const FVector& End);

private:

	AActor* _pTargetMagnetizableActor;	// 対象オブジェクト


	// オーバーレイマテリアルの設定方法
	// BPでも可能
	// この処理を持っておくオブジェクトは新しい奴を作る
	// 対象オブジェクトは、Typeインターフェースを持っているかどうかで判断
	// 対象のメッシュコンポーネントを取得して保持していたアウトラインマテリアルを適用
	// FunFun()
	// 点滅ディレイ = 0,点滅時間、間隔、色は各パラメータで制御
	// UMeshComponent->SetOverlayMaterial(Material);
};
