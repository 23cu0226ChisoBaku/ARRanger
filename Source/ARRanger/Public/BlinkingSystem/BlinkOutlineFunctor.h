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
	 * @brief 点滅終了時に呼ばれるコールバック
	 */ 
    TFunction<void(AActor*)> OnBlinkEnd;

	/**
	 * @brief 点滅処理を行う関数
	 */
	void OutlineBlink(AActor* targetObject, UMeshComponent* targetmeshComponent, FBlinkingActorData* blinkingData, float DeltaTime); 

	/**
	 * @brief 点滅処理を行う関数
	 *
	 * @param 対象アクター
	 */
	void ResetMaterialParam(AActor* targetActor); 

	/**
	 * @brief 指定されたパラメータに応じて等間隔で点滅を行う
	 * 
	 * @param 
	 */
	void ConstantBlink(AActor* targetObject, float blinkInterval, float blinkSpeed);

	/**
	 * @brief 指定されたパラメータに応じて徐々に早くなる点滅を行う
	 * 
	 * @param 
	 */
	void AcceleratedBlink(AActor* targetObject, float blinkInterval, float blinkSpeed);

	/**
	 * @brief 動的なマテリアルを生成する
	 * 
	 * @param 点滅させるマテリアル
	 */
	UMaterialInstanceDynamic* CreateDynamicMaterial(UMaterialInterface* blinkMaterial, UObject* targetObject);


};
