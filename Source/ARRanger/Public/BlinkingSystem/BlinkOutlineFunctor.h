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

	/**
	 * @brief 指定されたパラメータに応じて等間隔で点滅を行う
	 * 
	 * @param 
	 */
	void ConstantBlink();

	/**
	 * @brief 指定されたパラメータに応じて徐々に早くなる点滅を行う
	 * 
	 * @param 
	 */
	void AcceleratedBlink();
};
