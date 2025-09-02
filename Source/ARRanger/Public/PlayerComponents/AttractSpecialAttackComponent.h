//*************************************************
// 引力必殺技の制御コンポーネント
//*************************************************
#pragma once

#include "Components/ActorComponent.h"
#include "AttractSpecialAttackComponent.generated.h"


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

private:

	/**
	 * @brief 対象のオブジェクトを引き寄せる物体を生成する
	 */
	void GeneratAttractActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|Player", meta = (AllowPrivateAccess = "true"))
	float m_AttractTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractSpecialAttack|AttractionActor", meta = (AllowPrivateAccess = "true"))
	FVector m_OffsetPositionOfAttractionActor;

	UPROPERTY()
    float m_ElapsedTime;
	UPROPERTY()
	bool m_IsAttractSpecialAttack;
};