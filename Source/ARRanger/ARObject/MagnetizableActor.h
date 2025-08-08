//*************************************************
// 斥力引力の干渉を受けるオブジェクトのベースクラス
//*************************************************

#pragma once

#include "GameFramework/Actor.h"
#include "Public/IARMagnetizableInterface.h"

#include "MagnetizableActor.generated.h"

UCLASS(Abstract)
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
	UFUNCTION(BlueprintCallable)
	static void CallAttraction(AMagnetizableActor* actor)
	{
		actor->OnAttraction();
	}

	/**
	 * @brief 斥力が付与されている状態の挙動
	 */
	UFUNCTION(BlueprintCallable)
	static void CallRepulsion(AMagnetizableActor* actor)
	{
		actor->OnRepulsion();
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

	/*Start IARMagnetizableInterface interface*/
	virtual void OnAttraction() override;
	virtual void OnRepulsion() override;
	virtual void OnMagneticForceEvaluated(const FVector& magneticForce) override;
	virtual AActor* GetActor() override { return (AActor*)this; }
	/*End IARMagnetizableInterface interface*/

	/*テスト用*/
	UFUNCTION(BlueprintCallable)
	void SetType(EARMagnetismType newType);

	UFUNCTION(BlueprintPure)
	EARMagnetismType GetType() const;

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
