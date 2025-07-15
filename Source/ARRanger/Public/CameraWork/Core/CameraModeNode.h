#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_MODE_NODE_
#define _MCW_CAMERAWORK_CORE_CAMERA_MODE_NODE_

#include "Core/CoreDefines.h"

namespace AR
{
namespace CameraWork
{

  class FCameraModeNodeEvaluator;
  using FCameraModeNodeEvaluatorPtr = AR::CameraWork::FCameraModeNodeEvaluator*;

  class FCameraModeNode
  {

    public:
      FCameraModeNode();
      ~FCameraModeNode() = default;

      MCW_API FCameraModeNodeEvaluatorPtr MakeEvaluator() const;
      MCW_API bool IsEnable() const;

    protected:
      virtual FCameraModeNodeEvaluatorPtr MakeEvaluatorImpl() const = 0;

    private:
      bool m_bIsEnabled : 1;
  };
  
} // namespace AR::CameraWork
} // namespace AR 

#endif // _CAMERAWORK_CORE_CAMERA_MODE_NODE_