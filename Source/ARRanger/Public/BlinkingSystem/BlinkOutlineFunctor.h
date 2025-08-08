//*************************************************
// アウトラインの点滅処理クラス
//*************************************************

#pragma once

// 前方宣言
struct FBlinkingActorData;

class BlinkOutlineFunctor
{
public:
	BlinkOutlineFunctor();
	~BlinkOutlineFunctor();

	/**
	 * @brief 点滅処理を行う関数
	 */
	void OutlineBlink(FBlinkingActorData* blinkingData, float DeltaTime, UObject* targetObject); 

private:

	/**
	 * @brief 動的なマテリアルを生成する
	 * 
	 * @param 点滅させるマテリアル
	 */
	UMaterialInstanceDynamic* CreateDynamicMaterial(UMaterialInterface* blinkMaterial, UObject* targetObject);

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> m_DynamicMaterial;	/*動的マテリアル*/
};
