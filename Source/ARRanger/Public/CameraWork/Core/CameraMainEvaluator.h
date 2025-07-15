#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MAIN_EVALUATOR_
#define _MCW_CAMERAWORK_CORE_CAMERA_MAIN_EVALUATOR_

#include "Core/CoreDefines.h"
#include "Core/CameraModeNodeEvaluator.h"
#include "Core/CameraModeView.h"

#include <memory>

/**
 * 前方宣言
 */

namespace AR
{
namespace CameraWork
{
  class FCameraModeRootNode;
  class FCameraModeRootNodeEvaluator;

  // NOTE: Private alias
  // NOTE: Current environment: Unreal engine 5.6
  #if IS_UE_ENVIRONMENT
    namespace Private
    {
      using OwnerType = UObject;
      using StrongOwnerPtr = TObjectPtr<OwnerType>;
      using WeakOwnerPtr = TWeakObjectPtr<OwnerType>;

      template<typename T>
      using SharedPtr = TSharedPtr<T>;

      template<typename T>
      using WeakPtr = TWeakPtr<T>;

      // Factory delegate
      using RootNodeFactoryMethod = TDelegate<FCameraModeRootNode*()>;
    }
  #else
    namespace Private
    {

    }
  #endif

  struct FCameraMainEvaluatorInitializeParameters
  {
    /**
     * Owner of Main evaluator, 
     */
    Private::StrongOwnerPtr Owner;

    /**
     * Optional root node override factory method
     * Can be null
     */
    Private::RootNodeFactoryMethod RootNodeFactory;
  };

  struct FCameraMainEvaluationParameters
  {
    /**
     * Delta time in main game thread
     */
    float DeltaTime = 0.0f;
  };

  struct MCW_API FCameraMainEvaluationResult
  {
    FCameraModeView CameraView;

    public:
      void Reset();

      void Reset(const FCameraModeNodeEvaluationResult& OverrideResult);
  };

  /**
   * Main evaluator to evaluate nodes in specific camera work asset
   */
  class FCameraMainEvaluator
  {

    public:
      MCW_API FCameraMainEvaluator();
      MCW_API ~FCameraMainEvaluator();

      MCW_API void InitializeMainEvaluator(Private::StrongOwnerPtr = nullptr);
      MCW_API void InitializeMainEvaluator(const FCameraMainEvaluatorInitializeParameters& InitParams);

      MCW_API void Evaluate(const FCameraMainEvaluationParameters& EvaluateParams);

      MCW_API __forceinline const FCameraMainEvaluationResult& GetEvaluatedResult() const;

    private:
      void EvaluateImpl(const FCameraMainEvaluationParameters& EvaluateParams);
    
    private:
      Private::WeakOwnerPtr m_weakOwner;

      Private::SharedPtr<FCameraModeRootNode> m_rootNode;

      Private::SharedPtr<FCameraModeRootNodeEvaluator> m_rootEvaluator;

      /**
       * Result of root node evaluation
       */
      FCameraModeNodeEvaluationResult m_nodeResult;

      /**
       * Result of overall evaluation 
       */
      FCameraMainEvaluationResult m_evaluatedResult;
  };

  /**
   * Start FCameraMainEvaluator inline method definition
   */


  #pragma region Main evaluator inline method def
  __forceinline const FCameraMainEvaluationResult& FCameraMainEvaluator::GetEvaluatedResult() const
  {
    return m_evaluatedResult;
  }
  #pragma endregion Main evaluator inline method def

  /**
   * End FCameraMainEvaluator inline method definition
   */
}
}

#endif // _MCW_CAMERAWORK_CORE_CAMERA_MAIN_EVALUATOR_