//*************************************************
// 引力必殺技の制御コンポーネント
//*************************************************
#pragma once

#include "Components/ActorComponent.h"
#include "AttractSpecialAttackComponent.generated.h"

/*前方宣言*/
class UGameplayCameraComponent;
class ASpecialAttackAttractActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API UAttractSpecialAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttractSpecialAttackComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

	/**
	 * @brief コンポーネント所有者についているカメラコンポ―ネントを取得する関数
	 * 
	 * @param コンポーネント所有者についているカメラコンポ―ネント
	 */
	UFUNCTION(BlueprintCallable)
	void SetPlayerCameraComponent(const UGameplayCameraComponent* playerCameraComp)
	{
		if (playerCameraComp)
		{
			m_PlayerCameraComponent = const_cast<UGameplayCameraComponent*>(playerCameraComp);
		}
	}

	/**
	 * @brief 引力必殺技を始めた際の処理
	 */
	UFUNCTION(BlueprintCallable)
	void StartSpecialAttract();

  UFUNCTION(BlueprintCallable)
  void PerformSpecialAttract();

  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialAttractFinishedDelegate);
  UPROPERTY(BlueprintAssignable)
  FOnSpecialAttractFinishedDelegate OnSpecialAttractFinished;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_IsGenerateAttract;      		/*引力アクターを生成しているか*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_IsBackFlip;			      	/*バックフリップをしているか*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_IsAttractKick;      			/*キックをしているか (for 麦)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_IsLand;      				/*着地しているか　(for 麦)*/

private:
	/**
	 * @brief 対象のオブジェクトを引き寄せる物体を生成する
	 */
	void GenerateAttractActor();

	/**
	 * @brief 対象のアクターを引き寄せている場所にキック!!
	 * 
	 * @param １フレームにかかる時間
	 */
	void SpecialFinishKick(float deltaTime);

	/**
	 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(後からなくなる)
	 */
	FVector GetPlayerCameraRotation();

	/**
	 * @brief 引力必殺技に関するパラメータをリセットする関数
	 */
	void ResetParameter();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true", DisplayName = "CustomCurveSpeed"))
  UCurveFloat* m_KickCurveSpeed;        	/*フィニッシュキックのスピード*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> m_AttractActor;     /*対象オブジェクトを吸引オブジェクト*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_AttractTimeInterval;            /*技発動からキックし始めるまでの時間*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_KickTimeInterval;               /*キックし始めから着地までの時間*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_LandTimeInterval;               /*着地から動き始めるまでの時間*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_KickMaxSpeed;                   /*最大キックスピード*/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_KickBrakingForce;               /*キックの勢いを止める力*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	float m_GenerateDistance;               /*吸引オブジェクト生成する際のプレイヤーとの距離*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_KickHitDetectionLength;         /*キック時の衝突判定用ライントレースの長さ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	float m_GenerateDistanceOffset;         /*生成位置が衝突していた際の調整距離*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
  	float m_KickPositionOffset;        		/*キック時に障害物に当たった時のプレイヤー座標調整用*/

	UPROPERTY()
	TArray<AActor*> m_InhaledActors;			/*吸引しているアクター*/
	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> m_PlayerCameraComponent;	/*プレイヤーについているカメラ*/
	UPROPERTY()
	TObjectPtr<AActor> m_GenerateArractActor;   /*生成した吸引オブジェクト*/
	UPROPERTY()
	FVector m_kickDirection;					/*キックをする方向*/
	UPROPERTY()
  	float m_CurrentKickSpeed;           		/*現在のキックスピード*/
	UPROPERTY()
  	float m_ElapsedTime;                		/*経過時間計測用*/
};