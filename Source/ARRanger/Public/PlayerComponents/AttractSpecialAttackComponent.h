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
	 * @brief Playerについているカメラの回転(Rotation) を取得するための関数(後からなくなる)
	 */
	FVector GetPlayerCameraRotation();

	/**
	 * @brief 対象のオブジェクトを引き寄せる物体を生成する
	 */
	void GeneratAttractActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_AttractTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))//, MustImplement = "ARAttackable"))
	TSubclassOf<ASpecialAttackAttractActor> m_AttractActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	float m_GeneratDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	float m_OffsetGeneratDistance;

	UPROPERTY()
	TObjectPtr<UGameplayCameraComponent> m_PlayerCameraComponent;	/*プレイヤーについているカメラ*/
	UPROPERTY()
    float m_ElapsedTime;
	UPROPERTY()
	bool m_IsAttractSpecialAttack;
};