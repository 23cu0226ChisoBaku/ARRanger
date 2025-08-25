// //*************************************************
// // アウトラインの点滅処理を制御するシステム
// //*************************************************

// #pragma once

// // 前方宣言
// class AActor;
// class UWorld;
// class ABlinkOutlineTickActor;

// class FBlinkingOutlineSystem
// {
// public:

// 	FBlinkingOutlineSystem();
// 	~FBlinkingOutlineSystem() = default;

// 	/*
// 	* @brief アウトラインの点滅処理を行うアクターの生成
// 	*/
// 	void CreateTickingActor(UWorld* world, TSubclassOf<ABlinkOutlineTickActor> Subclass = nullptr);

// 	/*
// 	* @brief アウトラインの処理を呼び出すデリゲート関数
// 	*
// 	* @param アウトラインをつける対象のオブジェクトポインタ 
// 	*/
// 	void SetActorOnBlinkingOutline(AActor* targetMagnetizableObject);

// 	/*
// 	* @brief アウトラインの処理を終了させるデリゲート関数
// 	*
// 	* @param アウトラインをはずす対象のオブジェクトポインタ
// 	*/
// 	void UnsetActorOnBlinkingOutline(AActor* targetMagnetizableObject);

// private:

// 	TWeakObjectPtr<ABlinkOutlineTickActor> m_TickActor;	// 点滅処理を駆動するためのアクター
// };

