//*************************************************
// 引力必殺技の通知を管理するインターフェース
//*************************************************
#pragma once

#include "UObject/Interface.h"
#include "ISpecialAttractInterface.generated.h"

UINTERFACE(MinimalAPI)
class USpecialAttractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 引力必殺技を管理するインターフェース
 */
class ARRANGER_API ISpecialAttractInterface
{
	GENERATED_BODY()
public:
	/** 
	 * @brief 引力必殺技が始まった時の通知 
	 */
	virtual void OnStartSpecialAttractNotify(){;}

	/**
	 * @brief 引力必殺技の中間通知 
	 * 
	 * @param 経過時間
	 */
	virtual void OnUpdateSpecialAttractNotify(float elapsed){;}

	/**
	 * @brief 引力必殺技の終了通知
	 */
	virtual void OnEndSpecialAttractNotify(){;}
};
