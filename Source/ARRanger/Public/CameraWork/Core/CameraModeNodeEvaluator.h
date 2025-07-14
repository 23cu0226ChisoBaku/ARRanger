#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MODE_NODE_EVALUATOR_
#define _MCW_CAMERAWORK_CORE_CAMERA_MODE_NODE_EVALUATOR_

#include "Core/CoreDefines.h"

#include <type_traits>

namespace AR
{
namespace CameraWork
{
  class FCameraModeNode;
  class FCameraModeEvaluationResult;

  class FCameraModeNodeEvaluator
  {
    friend class FCameraModeNode;

    public:
      MCW_API FCameraModeNodeEvaluator() = default;
      MCW_API virtual ~FCameraModeNodeEvaluator() = default;

    public:
      MCW_API const FCameraModeNode* GetCameraModeNode() const;
      
      template<typename NodeType>
      const NodeType* GetCameraModeNodeAs() const;

      MCW_API void Evaluate(FCameraModeEvaluationResult& Result);

    protected:
      MCW_API virtual void EvaluateImpl(FCameraModeEvaluationResult& Result);
    
    // Only for internal use
    private:
      void SetCameraModeNode(const FCameraModeNode* Node);
    
    private:
      const FCameraModeNode* m_privateNode;
  };

  template<typename NodeType>
  const NodeType* FCameraModeNodeEvaluator::GetCameraModeNodeAs() const
  {
    return CastFast<NodeType>(m_privateNode);
  }

  template<typename NodeType>
  class TCameraModeNodeEvaluator : private FCameraModeNodeEvaluator
  {
    static_assert(std::is_base_of_v<FCameraModeNode, NodeType>, "Use non devired class of FCameraModeNode is invalid");

    const NodeType* GetCameraModeNode() const
    {
      return GetCameraModeNodeAs<NodeType>();
    }
  };

} // namespace AR::CameraWork
} // namespace AR

#endif // _CAMERAWORK_CORE_CAMERA_MODE_NODE_EVALUATOR_