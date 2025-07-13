#include "Core/CameraNode.h"

namespace AR
{
namespace CameraWork
{
  FCameraModeNodeEvaluatorPtr FCameraModeNode::MakeEvaluator() const
  {
    FCameraModeNodeEvaluatorPtr evaluator = MakeEvaluatorImpl();
    check(evaluator != nullptr);

    return evaluator;
  }

  FCameraModeNodeEvaluatorPtr FCameraModeNode::MakeEvaluatorImpl() const
  {
    return nullptr;
  }

}
}