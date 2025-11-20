/**
 * @file IARAttackable.h
 * @brief Interface for object that can be attacked in ARRanger project 
 */
/**
 * @brief 攻撃を受けることができるオブジェクト用のインターフェイス
 */

#pragma once

#include "UObject/Interface.h"

#include "IARAttackable.generated.h"

#define UE_API ARRANGER_API

namespace ARRanger
{

namespace Battle
{
  /**
   * @brief Type of attack result
   * ※ Success: Attack succeeded
   *    Inmune : Attack does not work
   *    Rebound: Attack rebounds
   */
  /**
   * @brief 攻撃結果列挙型
   * ※ Success: 攻撃成功
   *    Inmune : 攻撃が通らない（相手が無敵）
   *    Rebound: 攻撃が跳ね返る
   */
  enum struct EARAttackResult : uint8
  {
    Success,  
    Inmune,   
    Rebound,  
  };

  /**
   * @brief Parameter structure of attack result 
   */
  /**
   * @brief 攻撃結果構造体
   */
  struct FARAttackResult
  {
    EARAttackResult Result = EARAttackResult::Inmune;
  };
  
  /**
   * @brief Parameter structure of damage result
   */
  /**
   * @brief ダメージ結果構造体
   */
  struct FARDamageResult
  {
    /**攻撃元 */
    TObjectPtr<AActor> Instigator;
    
    /**攻撃の向き */
    FVector FinalLaunchDirection = FVector::ZeroVector;
    
    /**攻撃が当たる座標 */
    FVector ImpactLocation = FVector::ZeroVector;

    /**ダメージ */
    float FinalDamage = 0.0f;
  };
} // namespace ARRanger::Battle

} // namespace ARRanger


/**
 * @brief Parameter structure of attack behaviour
 */
/**
 * @brief 攻撃パラメータ構造体
 */
USTRUCT(BlueprintType)
struct FARAttackParameters
{
  GENERATED_BODY()
  
  /**攻撃元 */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  TObjectPtr<AActor> Instigator;
  
  /**飛ばす方向(単位ベクトル) */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  FVector LaunchDirection;
  
  /**攻撃の与える座標 */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  FVector ImpactLocation;
  
  /**ダメージ */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  float Damage;
  
  /**
   * 攻撃者のアクターを使用するか
   * falseかつInstigatorが有効だったらInstigatorを使い、それ以外は攻撃者のActorを使う
   */
  UPROPERTY(EditDefaultsOnly, Category = "Parameters|Attack")
  bool bUseAttackerActor;

  UE_API FARAttackParameters();

  /**空白攻撃パラメータ（無効値） */
  UE_API static const FARAttackParameters BlankAttackParams;
  static FARAttackParameters GetBlank() { return BlankAttackParams; }
};

/**Forward declaration */
class IARAttackerInterface;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UARAttackable : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class IARAttackable
 * @brief Interface for object that can be attacked in ARRanger project 
 */
class IARAttackable
{
	GENERATED_BODY()

public:

  /**
   * @brief                 物を攻撃する
   * @param Attacker        攻撃者
   * @param InAttackParams  攻撃パラメータ
   * @return 成功だったらtrue, それ以外はfalseを返す
   */
  UE_API bool AttackTarget(IARAttackerInterface* Attacker, FARAttackParameters InAttackParams = FARAttackParameters::GetBlank());

  /**
   * @brief   アクターを返す
   * @return  AActor
   */
  UE_API virtual AActor* Attackable_GetActor() { return ::Cast<AActor>(_getUObject()); }

  /**
   * @brief   攻撃できるか
   * @return  攻撃できたらtrue, それ以外はfalse
   */
  UE_API virtual bool CanAttack() { return true; }

protected:

  /**
   * @brief 攻撃を受ける前に呼び出されるコールバック
   * @param InAttackParams  @see FARAttackParameters
   * @param OutAttackResult @see ARRanger::Battle::FARAttackResult
   */
  UE_API virtual void OnPreAttacked(const FARAttackParameters& InAttackParams, ARRanger::Battle::FARAttackResult& OutAttackResult) { }

  /**
   * @brief 攻撃を受けた後に呼び出されるコールバック
   * @param InAttackParams  @see FARAttackParameters
   */
  UE_API virtual void OnPostAttacked(const FARAttackParameters& InAttackParams) { }

  /**
   * @brief ダメージ計算を済んだ時に呼び出されるコールバック
   * @param InDamageResult @see ARRanger::Battle::FARDamageResult
   */
  UE_API virtual void OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult) { }
};

#undef UE_API