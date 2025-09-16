//*************************************************
// 引力必殺技の対象オブジェクトを吸引するアクター
//*************************************************
#pragma once

#include "GameFramework/Actor.h"
#include "ISpecialAttractInterface.h"
#include "SpecialAttackAttractActor.generated.h"

UCLASS()
class ARRANGER_API ASpecialAttackAttractActor : public AActor,
                                                public ISpecialAttractInterface
{
	GENERATED_BODY()
public:	
	ASpecialAttackAttractActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	/**
	 * @brief 吸引しているアクターを取得する関数
	 * 
	 * @return 吸引しているアクター
	 */
	const TArray<AActor*> GetDetectedActors() const { return m_PreviousDetectedActors;}

private:

    /**
     * @brief 引力必殺技の開始処理
     */
    void OnStartSpecialAttact();

    /**
     * @brief 引力必殺技の開始処理
     */
    void OnEndSpecialttact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Target", meta = (AllowPrivateAccess = "true", MustImplement = "ARAttackable"))
	TSet<TSubclassOf<AActor>> m_AttractionClassFilter;		/*検知したアクターへのフィルタ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Target", meta = (AllowPrivateAccess = "true"))
	TSet<TEnumAsByte<EObjectTypeQuery>> m_ObjectTypes;		/*検知するオブジェクトタイプ*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Attract", meta = (AllowPrivateAccess = "true"))
	float m_DetectionRadius;								/*吸い込み範囲*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Attract ", meta = (AllowPrivateAccess = "true"))
	float m_AttractSpeed;									/*中心に吸い込まれるスピード*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Attract", meta = (AllowPrivateAccess = "true"))
	float m_RotationSpeed;									/*回転して吸い込まれるスピード*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttractActor Paramater|Explosion", meta = (AllowPrivateAccess = "true"))
	float m_ExplosionPower;									/*吸い込んだものを飛ばす力*/

    UPROPERTY()
	TArray<AActor*> m_PreviousDetectedActors;				/*吸い込み物を補完する*/
	UPROPERTY()
	TArray<AActor*> m_ResetSimulatePhysicsActors;			/*物理を解除したアクター保管用*/
};