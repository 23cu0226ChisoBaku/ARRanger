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

  void FCameraModeNodeEvaluator::Evaluate(FCameraModeEvaluationResult& Result)
  {
    if ((m_privateNode != nullptr) && m_privateNode->IsEnable())
    {
      EvaluateImpl(Result);
    }
  }
  
  void FCameraModeNodeEvaluator::EvaluateImpl(FCameraModeEvaluationResult& Result)
  {
    // Empty implementation
  }
    
  void FCameraModeNodeEvaluator::SetCameraModeNode(const FCameraModeNode* Node)
  {
    m_privateNode = Node;
  }
}
}