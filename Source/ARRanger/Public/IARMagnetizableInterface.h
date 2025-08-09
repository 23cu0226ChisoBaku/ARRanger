//*************************************************
// 斥力・引力の磁気性質を管理するインターフェース
//*************************************************

#pragma once

#include "UObject/Interface.h"
#include "IARMagnetizableInterface.generated.h"


/**
 * UObject専門のインターフェース
 */
UINTERFACE(MinimalAPI)
class UARMagnetizableInterface : public UInterface
{
	GENERATED_BODY()
};

/*
* 斥力引力状態(磁性)
*/
UENUM(BlueprintType)
enum class EARMagnetismType : uint8
{
	None = 0,		// 何もない状態(デフォルト値)
	Attraction = 1, // 引力状態
	Repulsion = 2,  // 斥力状態
};

/**
 * @brief Magnetic result from AR physics engine simulation
 */
struct FARMagneticForceResult
{
  /**磁力 */
  FVector FinalForce;
};


/**
 * 磁性を管理するインターフェース
 */
class IARMagnetizableInterface
{
	GENERATED_BODY()

public:

	/*
	* @brief 引力の磁性を保持している時の処理
	*/
	ARRANGER_API virtual void OnAttractionEvaluated(const FARMagneticForceResult& Result) {}

	/*
	* @brief 斥力の磁性を保持している時の処理
	*/
	ARRANGER_API virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) {}

	/*
	* @brief アクターを取得するための関数
	* 
	* @return アクターポインタ
	*/
	ARRANGER_API virtual AActor* GetActor() { return nullptr; }

	/*
	* @brief 新しい磁性に変化させる
	*
	* @return 変化先の磁性
	*/
	void SetMagnetismType(EARMagnetismType newARMagnetismType) { Type = newARMagnetismType; }
	
	/*
	* @brief 現在の磁性を取得する
	* 
	* @return 現在の磁性
	*/
	EARMagnetismType GetMagnetismType() const { return Type; }

private:

	EARMagnetismType Type = EARMagnetismType::None;
};