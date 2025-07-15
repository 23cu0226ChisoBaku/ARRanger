#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MAIN_EVALUATOR_
#define _MCW_CAMERAWORK_CORE_CAMERA_MAIN_EVALUATOR_

#include "Core/CoreDefines.h"

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

    private:
      void EvaluateImpl(const FCameraMainEvaluationParameters& EvaluateParams);
    
    private:
      Private::WeakOwnerPtr m_weakOwner;

      Private::SharedPtr<FCameraModeRootNode> m_rootNode;

      Private::SharedPtr<FCameraModeRootNodeEvaluator> m_rootEvaluator;
  };
}
}

#endif // _MCW_CAMERAWORK_CORE_CAMERA_MAIN_EVALUATOR_