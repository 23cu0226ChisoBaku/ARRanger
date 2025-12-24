//*************************************************
// Hpバーの毎フレーム計算クラス
//*************************************************

#include "Tickable.h"

class HPBarCalcTiker : public FTickableObjectBase
{
publlic:
    Tick(float DeltaTime);

    /*
     * @brief 指定の値を指定された値で正規化
     * 
     * @param denominator:
     */
    float CalcHPBarNomalizeValue(const float& CalcNum, const float denominator, const float molecule);

    /* 
     * @brief 毎フレーム計算し始めるトリガー
     */
    void 
}