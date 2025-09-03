//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "Public/IARMagnetizableInterface.h"
#include "MagnetizableActor.generated.h"

UCLASS(Blueprintable)
class AMagnetizableActor :  public AActor, 
                            public IARMagnetizableInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:	
	AMagnetizableActor();
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief 引力が付与されている状態の挙動
	 */
	// DEPRECATED
	UFUNCTION(BlueprintCallable)
	static void CallAttraction(AMagnetizableActor* actor)
	{
		// FIXME Remove this
		FARMagneticForceResult result;
		actor->OnAttractionEvaluated(result);
	}

	/**
	 * @brief 斥力が付与されている状態の挙動
	 */
  	// DEPRECATED
	UFUNCTION(BlueprintCallable)
	static void CallRepulsion(AMagnetizableActor* actor)
	{
		// FIXME Remove this
		FARMagneticForceResult result;
		actor->OnRepulsionEvaluated(result);
	}

	/**
	 * @brief 斥力・引力の影響を受けれる状態かどうかを返す
	 * 
	 * @param 斥力・引力の影響を受けれる状態かどうか
	 */
	UFUNCTION(BlueprintCallable)
	bool CanMagneticForce()
	{
		return m_CanMagneticForce;
	}

	/**
	 * @brief 斥力・引力の状態変化が可能かどうかを返す
	 * 
	 * @param 斥力・引力の状態変化が可能かどうか
	 */
	UFUNCTION(BlueprintCallable)
	bool CanSetMagnetismType()
	{
		return m_CanSetMagnetismType;
	}

	/**
	 * @brief 斥力・引力の状態変化が可能かどうかを設定する
	 * 
	 * @param bCanSet 新しい状態（true/false）
	 */
	UFUNCTION(BlueprintCallable)
	void SetCanSetMagnetismType(bool bCanSet)
	{
		m_CanSetMagnetismType = bCanSet;
	}

	/*Start IARMagnetizableInterface interface*/
	virtual void OnAttractionEvaluated(const FARMagneticForceResult& Result) override;
	virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override;
	virtual AActor* GetActor() override { return (AActor*)this; }
	/*End IARMagnetizableInterface interface*/

	/*テスト用*/
	UFUNCTION(BlueprintCallable)
	void SetType(EARMagnetismType newType);

	// 妥協処理
	float ElapsedBlinkTime = 0.0f;
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicBlinkMaterial = nullptr;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* m_RootComponent;		// ピポット
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UPrimitiveComponent* m_MagneticField;	// 磁場範囲

	UPROPERTY()
	bool m_CanMagneticForce;				// 移動可能か
	UPROPERTY()
	bool m_CanSetMagnetismType;				// 磁性の設定が可能かどうか
};
