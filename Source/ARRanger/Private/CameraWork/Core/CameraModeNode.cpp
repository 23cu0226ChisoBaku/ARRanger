#include "Core/CameraModeNode.h"
#include "Core/CameraModeNodeEvaluator.h"

namespace AR
{
namespace CameraWork
{
  FCameraModeNode::FCameraModeNode()
    : m_bIsEnabled(false)
  { }

  FCameraModeNodeEvaluatorPtr FCameraModeNode::MakeEvaluator() const
  {
    FCameraModeNodeEvaluatorPtr evaluator = MakeEvaluatorImpl();
    check(evaluator != nullptr);
    evaluator->SetCameraModeNode(this);

    return evaluator;
  }

  bool FCameraModeNode::IsEnable() const
  {
    return m_bIsEnabled;
  }

  FCameraModeNodeEvaluatorPtr FCameraModeNode::MakeEvaluatorImpl() const
  {
    return nullptr;
  }

}
}