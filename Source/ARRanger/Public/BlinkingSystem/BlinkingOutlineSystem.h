//*************************************************
// アウトラインの点滅処理を制御するシステム
//*************************************************

#pragma once

// 前方宣言
class AActor;
class UWorld;
class ABlinkOutlineTickActor;

class FBlinkingOutlineSystem
{
public:

	FBlinkingOutlineSystem() = default;
	~FBlinkingOutlineSystem() = default;

	/*
	* @brief アウトラインの点滅処理を行うアクターの生成
	*/
	void CreateTickingActor(UWorld* world);

	// /*
	// * @brief LineTraceSingleARObjectComponent のデリゲート関数に登録
	// */
	// void RegisterDelegateFunction();

	/*
	* @brief アウトラインの処理を呼び出すデリゲート関数
	*
	* @param アウトラインをつける対象のオブジェクトポインタ 
	*/
	void SetTargetMagnetizableObjectDelegate(AActor* targetMagnetizableObject);

	/*
	* @brief アウトラインの処理を終了させるデリゲート関数
	*
	* @param アウトラインをはずす対象のオブジェクトポインタ
	*/
	void UnsetTargetMagnetizableObjectDelegate(AActor* targetMagnetizableObject);

private:

	ABlinkOutlineTickActor* m_TickActor;	// 点滅処理を駆動するためのアクター
};

