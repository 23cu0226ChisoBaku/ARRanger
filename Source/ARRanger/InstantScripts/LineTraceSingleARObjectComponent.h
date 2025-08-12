//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#pragma once

#include "Components/ActorComponent.h"

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
	 * @brief 対象としているオブジェクトを取得する
	 */
	UFUNCTION(BlueprintCallable)
	AActor* GetTargetMagnetizableActor(){ return m_TargetMagnetizableActor; }	

	/** 
	 * @brief BPでデリゲートを呼び出す
	 */
	UFUNCTION(BlueprintCallable)
	void ExecuteSetTargetMagnetizableObject();			

private:

	/**
	 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(多分後からなくなる)
	 */
	FVector GetPlayerCameraRotation();

	UPROPERTY()
	TObjectPtr<AActor> m_TargetMagnetizableActor;				// 対象オブジェクト
	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> m_PlayerCameraComp;	// プレイヤーのカメラ
};
