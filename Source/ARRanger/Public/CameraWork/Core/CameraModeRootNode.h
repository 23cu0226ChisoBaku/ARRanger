#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MODE_ROOT_NODE_
#define _MCW_CAMERAWORK_CORE_CAMERA_MODE_ROOT_NODE_

#include "Core/CameraModeNode.h"
#include "Core/CameraModeNodeEvaluator.h"

namespace AR
{
namespace CameraWork
{
  class FCameraRig;

  class FCameraModeRootNodeEvaluator : public FCameraModeNodeEvaluator
  {

    protected:
      MCW_API virtual void EvaluateImpl(FCameraModeNodeEvaluationResult& Result) override; 
  };

  class FCameraModeRootNode : public FCameraModeNode
  {
    protected:
      virtual FCameraModeNodeEvaluatorPtr MakeEvaluatorImpl() const override;
  };
}
}

#endif // _MCW_CAMERAWORK_CORE_CAMERA_MODE_ROOT_NODE_