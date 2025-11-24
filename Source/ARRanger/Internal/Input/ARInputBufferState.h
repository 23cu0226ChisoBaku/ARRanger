/**
 * @file ARInputBufferState.h
 * @author MAI ZHICONG
 * @brief 入力バッファ状態クラス
 */

#pragma once

#ifndef _INTERNAL_AR_INPUT_BUFFER_STATE_
#define _INTERNAL_AR_INPUT_BUFFER_STATE_

#define UE_API ARRANGER_API

#include "GameplayTagContainer.h"

/**前方宣言 */
class AARRangerPlayerController;

namespace ARRanger
{

namespace Input
{

/**
 * @brief 入力ステートステート
 */
class FARInputBufferState final
{
  // 内部ステート
  // 
  enum StateType
  {
    Pressed,  // 押された
    Released, // 離された
    Expired   // 有効切れ
  };

  // プライベートトークン（外部が直接FARInputBufferStateインスタンス作成できないかつMakeShared/MakePimplなどに渡す）
  enum class PrivateToken{ };

public:

  /**
   * @brief 押された入力ステートを作成するファクトリー
   * @param InInputTag 入力Tag
   * @param BufferLifeTime 入力ステート有効時間
   * 
   * @return 作成した押された入力ステート 
   */
  static UE_API TPimplPtr<FARInputBufferState> MakePressedState(const FGameplayTag& InInputTag, float BufferLifeTime);
  
  /**
   * @brief 離れた入力ステートを作成するファクトリー
   * @param InInputTag 入力Tag
   * @param BufferLifeTime 入力ステート有効時間
   * 
   * @return 作成した離れた入力ステート 
   */ 
  static UE_API TPimplPtr<FARInputBufferState> MakeReleasedState(const FGameplayTag& InInputTag, float BufferLifeTime);

  UE_API FARInputBufferState(PrivateToken);
  UE_API ~FARInputBufferState();

  // TODO maybe Evaluate should only use DeltaTime?
  /**
   * @brief 入力ステートを評価する
   * @param InPlayerController プレイヤーコントローラー
   * @param DeltaTime
   */
  UE_API void Evaluate(AARRangerPlayerController* InPlayerController, float DeltaTime);

  /**
   * @brief 入力ステートを次の状態に進行する
   * (Pressed->Released, Released->Expired, Expired->Expired)
   */
  UE_API void AdvanceInputState();

  /**
   * @brief ステート残り有効時間を返す
   * @return 残り有効時間(常に0.0f以上)
   */
  UE_API float GetLifeTime() const;

  /**
   * @brief ステート残り有効時間を設定する
   * @param NewBufferLifeTime
   */
  UE_API void SetLifeTime(float NewBufferLifeTime);


  /**
   * @brief 入力ステートのTagと全く一緒かを確認する
   * @param InInputTag
   * @return 全く一緒だったらtrue、それ以外false
   */
  UE_API bool IsInputTagMatchesExact(const FGameplayTag& InInputTag) const;

  // TODO
  /**
   * @brief Check if this is pressed input state
   * @return True if input state is Pressed, otherwise false.
   */
  bool IsPressedState() const { return m_inputState == Pressed; } 

  /**
   * @brief Check if this is released input state
   * @return True if input state is Released, otherwise false.
   */
  bool IsReleasedState() const { return m_inputState == Released; } 
  
  /**
   * @brief ステートが破棄されたかを確認する
   * 
   * @return 破棄されたらtrue、それ以外false
   */
  bool IsExpiredState() const { return m_inputState == Expired; }


  /**
   * @brief この入力ステートを押された入力ステートにマークする
   */
  UE_API void MarkAsPressed();
  /**
   * @brief この入力ステートを離れた入力ステートにマークする
   */
  UE_API void MarkAsReleased();

private:

  static TPimplPtr<FARInputBufferState> MakeInstanceInternal();
  void CheckInputStateValidation();

private:

  /**バッファ残り有効時間 */
  float m_bufferLifeTime;

  /**入力タグ */
  FGameplayTag m_inputTag;

  /**（内部）入力ステート種類 */
  StateType m_inputState;

};

} // namespace ARRanger::Input

} // namespace ARRanger

#undef UE_API

#endif
