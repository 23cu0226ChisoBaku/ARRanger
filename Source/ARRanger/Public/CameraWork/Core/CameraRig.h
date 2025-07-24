#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_RIG_
#define _MCW_CAMERAWORK_CORE_CAMERA_RIG_

#include "Core/CoreDefines.h"

#include "Misc/EnvironmentTypeAdapter.h"

namespace AR
{
namespace CameraWork
{
  // Forward declaration
  class FCameraModeNode;

  /**
   * Camera rig for camera control
   */
  class FCameraRig
  {
    private:
      using Node = AR::CameraWork::FCameraModeNode;

    private:
      Private::SharedPtr<Node> m_rootNode;
  };
}
}

#endif // _MCW_CAMERAWORK_CORE_CAMERA_RIG_