/**
 * MCW Camera main evaluator
 * 
 * Unreal engine implementation
 */

#include "Core/CameraMainEvaluator.h"

#include "Core/CameraModeRootNode.h"

namespace AR
{
namespace CameraWork
{
  FCameraMainEvaluator::FCameraMainEvaluator()
    : m_weakOwner{nullptr}
    , m_rootNode{nullptr}
    , m_rootEvaluator{nullptr}
  { }

  FCameraMainEvaluator::~FCameraMainEvaluator()
  { 
    // NOTE: Empty implementation
  }

  void FCameraMainEvaluator::InitializeMainEvaluator(Private::StrongOwnerPtr OwnerPtr)
  {
    FCameraMainEvaluatorInitializeParameters params;
    params.Owner = OwnerPtr;

    InitializeMainEvaluator(params);
  }

  void FCameraMainEvaluator::InitializeMainEvaluator(const FCameraMainEvaluatorInitializeParameters& InitParams)
  {
    Private::StrongOwnerPtr owner = InitParams.Owner;

    if (owner == nullptr)
    {
      owner = GetTransientPackage();
    }
    
    m_weakOwner = owner;

    if (InitParams.RootNodeFactory.IsBound())
    {
      m_rootNode = Private::SharedPtr<FCameraModeRootNode>{InitParams.RootNodeFactory.Execute()};
    }
    else
    {
      m_rootNode = Private::SharedPtr<FCameraModeRootNode>{new FCameraModeRootNode()};
    }

    // Build evaluator
    {
      // TODO: This can make a function
      m_rootEvaluator = Private::SharedPtr<FCameraModeRootNodeEvaluator>{
                          static_cast<FCameraModeRootNodeEvaluator*>(m_rootNode->MakeEvaluator())
                        };
      if (m_rootEvaluator != nullptr)
      {
        FCameraModeNodeEvaluatorBuildParameters buildParams;

        m_rootEvaluator->BuildEvaluator(buildParams);
      }
    }

    // Initialize evaluator
    {
      if (m_rootEvaluator != nullptr)
      {
        m_rootEvaluator->InitializeEvaluator();
      }
    }
  }

  void FCameraMainEvaluator::Evaluate(const FCameraMainEvaluationParameters& EvaluateParams)
  {
    EvaluateImpl(EvaluateParams);
  }

  void FCameraMainEvaluator::EvaluateImpl(const FCameraMainEvaluationParameters& EvaluateParams)
  {
    if 
  }
}
}
