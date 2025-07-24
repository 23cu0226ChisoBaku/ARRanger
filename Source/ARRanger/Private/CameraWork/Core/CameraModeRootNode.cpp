#include "Core/CameraModeRootNode.h"

namespace AR
{
namespace CameraWork
{
  void FCameraModeRootNodeEvaluator::EvaluateImpl(FCameraModeNodeEvaluationResult& Result)
  {
    // FIXME: UE 5.6 TEST CODE
    if (GEngine)
    {
      
    }
  }

  FCameraModeNodeEvaluatorPtr FCameraModeRootNode::MakeEvaluatorImpl() const
  {
    // NOTE: Currently for temporary use. Need change static to memory management
    static FCameraModeRootNodeEvaluator s_evaluator;
    return &s_evaluator;

  }
} 
}