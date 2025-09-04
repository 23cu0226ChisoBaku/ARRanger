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

	/**
	 * @brief 引力必殺技を発動中かどうかを取得
	 * 
	 * @return 引力必殺技が発動中かどうか
	 */
	bool GetIsAttractSpecialAttack()const{return m_IsAttractSpecialAttack;}

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

private:

	/**
	 * @brief 引力必殺技を始めた際の処理
	 */
	UFUNCTION(BlueprintCallable)
	void OnStartSpecialAttract();

	/**
	 * @brief 対象のオブジェクトを引き寄せる物体を生成する
	 */
	void GenerateAttractActor();

	/**
	 * @brief 対象のアクターを引き寄せている場所にキック!!
	 */
	void SpecialFinishKick();

	/**
	 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(後からなくなる)
	 */
	FVector GetPlayerCameraRotation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_KickSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_AttractTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	ASpecialAttackAttractActor* m_AttractActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	float m_GenerateDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	float m_OffsetGenerateDistance;

	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> m_PlayerCameraComponent;	/*プレイヤーについているカメラ*/
	UPROPERTY()
	FTimerHandle m_DelayTimerHandle;
	UPROPERTY()
    float m_CurrentKickSpeed;
	UPROPERTY()
    float m_ElapsedTime;
	UPROPERTY()
	bool m_IsAttractSpecialAttack;
};