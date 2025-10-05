//*************************************************
// 引力フックショット(仮)の挙動を行うコンポーネント
//*************************************************

#pragma once

#include "Components/ActorComponent.h"
#include "HookshotComponent.generated.h"

/*フックショットのスピードの変わり方*/
UENUM(BlueprintType)
enum class EHookshotSpeedCurve : uint8
{
    Linear      UMETA(DisplayName="Linear"),                /*一定速度で上昇*/ 
    Exponential UMETA(DisplayName="Exponential"),           /*だんだん増加が緩やかに*/ 
    Logarithmic UMETA(DisplayName="Logarithmic"),           /*初め急に上がり後で緩やかに*/  
    CurveAsset  UMETA(DisplayName="Custom | CurveAsset")    /*カーブアセットの*/
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHookshotComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:	
	UHookshotComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		

    /**
     * @brief BPから呼ぶフックショット開始関数
     * 
     * @param targetActor 移動先ターゲット
     */
    UFUNCTION(BlueprintCallable)
    void StartHookshot(AActor* targetActor);

    /**
     * @brief フックショット中の毎フレーム処理
     * 
     * @param DeltaTime 1フレームにかかる時間
     */
    void HookshotAction(float DeltaTime);

    /**
     * @brief 始点から終点に対しての方向ベクトルを計算
     * 
     * @param 始点,終点
     * 
     * @return 正規化した方向ベクトル
     */
    UFUNCTION()
    FVector CalculationDirection(FVector StartPos, FVector EndPos);

    /**
     * @brief フックショット状態かどうかを返す
     * 
     * @param フックショット状態かどうか
     */
    bool IsHookShot(){ return m_IsHookshotAction;}

private:

    /**
     * @brief フックショットの移動と障害物判定を行う
     * 
     * @param  コンポーネント所有者の座標 ,進行方向ベクトル(正規化済み)
     * 
     * @return フックショットをやめるかどうか
     */
    bool HitCheckOnHookshot(const FVector& ownerLocation ,const FVector& direction);

    /**
     * @brief フックショットのスピードを徐々に上げる関数
     * 
     * @param 1フレームにかかる時間
     */
    UFUNCTION()
    void IncreaseHookshotSpeed(float deltaTime);

    /**
     * @brief フックショットをやめる際の処理
     */
    UFUNCTION()
    void StopHookshot();

    /** 
     * @brief TMap にスピード曲線用のラムダを登録する 
    */
    UFUNCTION()
    void SetupSpeedCurveFunctions();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Collision", meta = (AllowPrivateAccess = "true"))
    FVector m_HookshotBoxExtent;            /*フックショットを止めるためのBoxCollision*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Collision", meta = (AllowPrivateAccess = "true"))
    float m_HookshotBoxTraceDistance;       /*BoxCollision の検出距離*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Collision", meta = (AllowPrivateAccess = "true"))
    FVector m_HookshotBoxTraceOffset;       /*BoxCollision のオフセット*/ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Speed", meta = (AllowPrivateAccess = "true"))
    EHookshotSpeedCurve m_SpeedCurve;         /*フックショットのスピードタイプ*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Speed", meta = (AllowPrivateAccess = "true"))
    float m_HookshotSpeedIncreaseValue;     /*フックショットの加算スピード*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Speed", meta = (AllowPrivateAccess = "true"))
    float m_HookshotMaxSpeed;               /*フックショットの最高スピード*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hookshot|Speed", meta = (AllowPrivateAccess = "true"))
    float m_HookshotMinSpeed;               /*フックショットの最低スピード*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hookshot|Speed", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* m_CustomCurveSpeed;
    UPROPERTY()
    TObjectPtr<AActor> m_TargetActor;       /*ターゲットのアクター*/
    UPROPERTY()
    float m_CurrentHookshotSpeed;           /*現在のフックショットスピード*/
    UPROPERTY()
    float m_ElapsedTime;                    /*フックショットの経過時間*/
    UPROPERTY()
    float m_CurrentRatio;                   /*現在のプレイヤーからターゲットへの距離の比率*/
    UPROPERTY()
    bool m_CanHookshot;                     /*フックショットが出来るか*/
    UPROPERTY()
    bool m_IsHookshotAction;                /*フックショットを行っているか*/

    // フックショットのスピードタイプを切り替えるためのマップ
    TMap<EHookshotSpeedCurve, TFunction<void(float)>> SpeedCurveFunctions;
};