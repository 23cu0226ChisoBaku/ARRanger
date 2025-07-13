#pragma once

#ifndef _CAMERAWORK_CORE_CAMERA_MODE_NODE_
#define _CAMERAWORK_CORE_CAMERA_MODE_NODE_

#include "Core/CoreDefines.h"

namespace AR
{
namespace CameraWork
{

  class FCameraModeNodeEvaluator;
  using FCameraModeNodeEvaluatorPtr = FCameraModeNodeEvaluator*;

  class FCameraModeNode
  {
    public:
      FCameraModeNode();
      ~FCameraModeNode() = default;

      MCW_API FCameraModeNodeEvaluatorPtr MakeEvaluator() const;

    protected:
      virtual FCameraModeNodeEvaluatorPtr MakeEvaluatorImpl() const;

    private:
      bool bIsEnable : 1 = false;
  };
  
} // namespace AR::CameraWork
} // namespace AR 

#endif // _CAMERAWORK_CORE_CAMERA_MODE_NODE_