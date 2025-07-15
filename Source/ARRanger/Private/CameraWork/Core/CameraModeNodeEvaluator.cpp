#include "Core/CameraModeNodeEvaluator.h"
#include "Core/CameraModeNode.h"

namespace AR
{
namespace CameraWork
{
  const FCameraModeNode* FCameraModeNodeEvaluator::GetCameraModeNode() const
  {
    return m_privateNode;
  }

  void FCameraModeNodeEvaluator::BuildEvaluator(FCameraModeNodeEvaluatorBuildParameters& BuildParams)
  {
    BuildEvaluatorImpl(BuildParams);
  }

  void FCameraModeNodeEvaluator::InitializeEvaluator()
  {
    InitializeEvaluatorImpl();
  }

  void FCameraModeNodeEvaluator::Evaluate(FCameraModeNodeEvaluationResult& Result)
  {
    if ((m_privateNode != nullptr) && m_privateNode->IsEnable())
    {
      EvaluateImpl(Result);
    }
  }

  void FCameraModeNodeEvaluator::BuildEvaluatorImpl(FCameraModeNodeEvaluatorBuildParameters& BuildParams)
  {
    // Empty implementation
  }

  void FCameraModeNodeEvaluator::InitializeEvaluatorImpl()
  {
    // Empty implementation
  }
    
  void FCameraModeNodeEvaluator::SetCameraModeNode(const FCameraModeNode* Node)
  {
    m_privateNode = Node;
  }
}
}