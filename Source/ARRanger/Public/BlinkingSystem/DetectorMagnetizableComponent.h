//*************************************************
// 引力斥力を付与できるオブジェクトを取得するコンポーネント
//*************************************************

#pragma once

#include "Components/ActorComponent.h"
#include "DetectorMagnetizableComponent.generated.h"

/*前方宣言*/
class UGameplayCameraComponent;

/*デリゲート宣言*/
DECLARE_DELEGATE_OneParam(FStaticSetActorAtCursorDelegate, AActor*);
DECLARE_DELEGATE_OneParam(FStaticUnsetActorAtCursorDelegate, AActor*);
DECLARE_DELEGATE_OneParam(FStaticSetActorOnOutlineDelegate, AActor*);
DECLARE_DELEGATE_OneParam(FStaticUnsetActorOnOutlineDelegate, AActor*);
DECLARE_DELEGATE_OneParam(FStaticSetActorOnBlinkingOutlineDelegate, AActor*);
DECLARE_DELEGATE_OneParam(FStaticUnsetActorOnBlinkingOutlineDelegate, AActor*);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UDetectorMagnetizableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDetectorMagnetizableComponent();
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	/* 
	* @brief カーソルがあっているオブジェクトをシステムに渡すデリゲート
	*/
	FStaticSetActorAtCursorDelegate SetMagnetizableObjectAtCursor;

	/* 
	* @brief カーソルがあっているオブジェクトをシステムに渡すデリゲート
	*/
	FStaticUnsetActorAtCursorDelegate UnsetMagnetizableObjectAtCursor;

	/*
	* @brief 検知範囲内に入ったオブジェクトをシステムに渡すデリゲート
	*/
	FStaticSetActorOnOutlineDelegate SetActorOnOutline;

	/*
	* @brief 検知範囲外に出たオブジェクトをシステムに渡すデリゲート
	*/
	FStaticUnsetActorOnOutlineDelegate UnsetActorOnOutline;

	/*
	* @brief 引力斥力を付与するオブジェクトをシステムに設定するデリゲート
	*/
	FStaticSetActorOnBlinkingOutlineDelegate SetActorOnBlinkingOutline;

	/*
	* @brief 引力斥力を付与したをシステムから除外するデリゲート
	*/
	//FStaticUnsetActorOnBlinkingOutlineDelegate UnsetActorOnBlinkingOutline;

	/**
	 * @brief コンポーネント所有者についているカメラコンポ―ネントを取得する関数
	 * 
	 * @param コンポーネント所有者についているカメラコンポ―ネント
	 */
	UFUNCTION(BlueprintCallable)
	void SetPlayerCameraComponent(const UGameplayCameraComponent* playerCameraComp);

	/**
	 *  @brief 引力斥力を付与する対象のオブジェクトを他クラスに割り当てる処理
	 */
	UFUNCTION(BlueprintCallable)
	void AssignTargetMagnetizableObject();

	/**
	 *  @brief プレイヤーの状態(斥力・引力)をオブジェクトに付与する
	 * 
	 *  @param 付与する対象のアクター
	 */
	UFUNCTION(BlueprintCallable)
	void ApplyMagnetism(AActor* targetActor);

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
	AActor* GetTargetMagnetizableActor() { return m_TargetMagnetizableActor; }

private:

	/**
	 * @brief Playerを中心に指定した半径の磁性を持ったオブジェクトを検知
	 */
	void UpdateDetectedActors();

	/**
	 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(多分後からなくなる)
	 */
	FVector GetPlayerCameraRotation();

	UPROPERTY()
	TArray<TObjectPtr<AActor>> m_DetectedMagnetizableActors;		/*プレイヤー検知範囲内の磁性を持ったオブジェクト配列*/
	UPROPERTY()
	AActor* m_OwnerActor; 											/*プレイヤー(所有者)*/
	UPROPERTY()
	TObjectPtr<AActor> m_TargetMagnetizableActor;					/*ライントレースで検知したオブジェクト*/
	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> m_PlayerCameraComponent;	/*プレイヤーについているカメラ*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Detection Paramater")
    TArray<TSubclassOf<AActor>> m_DetectionClassFilter;		/*プレイヤー検知範囲内で検出するクラス*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Detection Paramater")
	TArray<TEnumAsByte<EObjectTypeQuery>> m_ObjectTypes;	/*プレイヤー検知範囲内で検出するオブジェクトタイプ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Detection Paramater")
	float m_DetectionRadius;								/*検出する範囲*/
};