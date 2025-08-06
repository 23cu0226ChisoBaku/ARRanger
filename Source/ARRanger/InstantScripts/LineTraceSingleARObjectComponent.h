//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#pragma once

#include "Components/ActorComponent.h"
#include "IARMagnetizableInterface.h"

#include "LineTraceSingleARObjectComponent.generated.h"

// 前方宣言
class UGameplayCameraComponent;

// デリゲート関数宣言
DECLARE_DELEGATE_OneParam(FStaticSetActorOnOutlineDelegate, AActor*);
DECLARE_DELEGATE_OneParam(FStaticUnsetetActorOnOutlineDelegate, AActor*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULineTraceSingleARObjectComponent : public UActorComponent
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
	* @brief 対象オブジェクトをシステム側に設定するデリゲート
	*/
	FStaticSetActorOnOutlineDelegate SetTargetMagnetizableObject;

	/*
	* @brief 対象オブジェクトをシステムから除外するデリゲート
	*/
	FStaticUnsetetActorOnOutlineDelegate UnsetTargetMagnetizableObject;

	/**
	 * @brief コンポーネント所有者についているカメラコンポ―ネントを取得する関数
	 * 
	 * @param コンポーネント所有者についているカメラコンポ―ネント
	 */
	UFUNCTION(BlueprintCallable)
	void SetPlayerCameraComp(const UGameplayCameraComponent* playerCameraComp);

	/**
	 *  @brief 引力斥力を付与する対象のオブジェクトを他クラスに割り当てる処理
	 */
	UFUNCTION(BlueprintCallable)
	void AssignTargetMagnetizableObject();	

	/*
	* @brief ライントレースを行って付与できるオブジェクトを検知
	*
	* @param ライントレースを行うための始点と終点
	*/
	UFUNCTION(BlueprintCallable, Category = "LineTrace")
	AActor* TraceForMagnetizableObject(const FVector& Start, const FVector& End);

	/** 
	 * @brief BPでデリゲートを呼び出す
	 */
	UFUNCTION(BlueprintCallable)
	void ExecuteSetTargetMagnetizableObject();

private:

	UPROPERTY()
	TObjectPtr<AActor> m_TargetMagnetizableActor;				// 対象オブジェクト
	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> m_PlayerCameraComp;	// プレイヤーのカメラ


	// オーバーレイマテリアルの設定方法
	// BPでも可能
	// この処理を持っておくオブジェクトは新しい奴を作る
	// 対象オブジェクトは、Typeインターフェースを持っているかどうかで判断
	// 対象のメッシュコンポーネントを取得して保持していたアウトラインマテリアルを適用
	// FunFun()
	// 点滅ディレイ = 0,点滅時間、間隔、色は各パラメータで制御
	// UMeshComponent->SetOverlayMaterial(Material);
};
