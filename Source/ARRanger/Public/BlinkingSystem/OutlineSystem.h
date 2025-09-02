//*************************************************
// アウトラインの表示処理を制御するシステム
//*************************************************

#pragma once

// 前方宣言
class AActor;
class UWorld;
class AOutlineTickActor;

class FOutlineSystem
{
public:

	FOutlineSystem();
	~FOutlineSystem() = default;

	/*
	* @brief アウトラインの点滅処理を行うアクターの生成
	*/
	void CreateTickingActor(UWorld* world, TSubclassOf<AOutlineTickActor> Subclass = nullptr);

	/*
	* @brief カーソルのあっているオブジェクトを白く点滅させるデリゲート関数
	*
	* @param 点滅するアウトラインをつける対象のオブジェクトポインタ
	*/
	void BlinkOutlineActorAtCursorDelegate(AActor* targetObject);

	/*
	* @brief カーソルが外れたオブジェクトの点滅を終了させるデリゲート関数
	*
	* @param 点滅するアウトラインをつける対象のオブジェクトポインタ
	*/
	void StopBlinkOutlineActorAtCursorDelegate(AActor* targetObject);

	/*
	* @brief アウトライン対象を追加するデリゲート関数
	*
	* @param アウトラインをつける対象のオブジェクトポインタ 
	*/
	void AddOutlineActorDelegate(AActor* targetObject);

	/*
	* @brief アウトライン対象を削除するデリゲート関数
	*
	* @param アウトラインをはずす対象のオブジェクトポインタ
	*/
	void RemoveOutlineActorDelegate(AActor* targetObject);

	/*
	* @brief 点滅アウトライン対象を追加するデリゲート関数
	*
	* @param 点滅するアウトラインをつける対象のオブジェクトポインタ 
	*/
	void AddBlinkingOutlineActorDelegate(AActor* targetObject);

	/*
	* @brief 点滅アウトライン対象を削除する
	*
	* @param 点滅するアウトラインをはずす対象のオブジェクトポインタ
	*/
	void RemoveBlinkingOutlineActor(AActor* targetObject);

    /*
	* @brief アウトライン対象配列の参照を返す
	*/
	const TArray<TWeakObjectPtr<AActor>>& GetOutlineActors() const { return m_OutlineActors; }

	/*
	* @brief 点滅アウトライン対象配列の参照を返す
	*/
	const TArray<TWeakObjectPtr<AActor>>& GetBlinkingOutlineActors() const { return m_BlinkingOutlineActors; }

private:

	TWeakObjectPtr<AOutlineTickActor> m_TickActor;	    	/*点滅処理を駆動するためのアクター*/ 

    UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> m_OutlineActors;	        /*アウトラインを付ける対象アクター配列*/ 
    UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> m_BlinkingOutlineActors; /*アウトラインを点滅させる対象アクター配列*/ 
};