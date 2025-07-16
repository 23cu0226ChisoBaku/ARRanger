#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MODE_NODE_EVALUATOR_
#define _MCW_CAMERAWORK_CORE_CAMERA_MODE_NODE_EVALUATOR_

#include "Core/CoreDefines.h"
#include "Core/CameraModeView.h"

#include <type_traits>

namespace AR
{
namespace CameraWork
{
  class FCameraModeNode;
  
  /**
   * Camera mode node evaluator build params
   * 
   * // NOTE:USE FOR MEMORY MANAGEMENT
   */
  struct FCameraModeNodeEvaluatorBuildParameters
  {
    
  };

  struct MCW_API FCameraModeNodeEvaluationResult
  {
    /**
     * Camera view for node evaluation
     */
    FCameraModeView CameraView;

    public:
      void Reset();
  };

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

      MCW_API void BuildEvaluator(FCameraModeNodeEvaluatorBuildParameters& BuildParams);

      MCW_API void InitializeEvaluator();

      MCW_API void Evaluate(FCameraModeNodeEvaluationResult& Result);

    // pure virtual method
    protected:
      MCW_API virtual void EvaluateImpl(FCameraModeNodeEvaluationResult& Result) = 0;
    
    // non-pure virtual method
    protected:
      MCW_API virtual void BuildEvaluatorImpl(FCameraModeNodeEvaluatorBuildParameters& BuildParams);

      MCW_API virtual void InitializeEvaluatorImpl();

    
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
  class TCameraModeNodeEvaluator : public FCameraModeNodeEvaluator
  {
    static_assert(std::is_base_of_v<FCameraModeNode, NodeType>, "Use non devired class of FCameraModeNode is invalid");

    public:
      const NodeType* GetCameraModeNode() const;
  };

  template<typename NodeType>
  const NodeType* TCameraModeNodeEvaluator<NodeType>::GetCameraModeNode() const
  {
    return GetCameraModeNodeAs<NodeType>();
  }


} // namespace AR::CameraWork
} // namespace AR

#endif // _CAMERAWORK_CORE_CAMERA_MODE_NODE_EVALUATOR_