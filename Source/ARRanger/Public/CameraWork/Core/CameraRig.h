#pragma once

#ifndef _MCW_CAMERAWORK_CAMERA_RIG_
#define _MCW_CAMERAWORK_CAMERA_RIG_

#include <memory>

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
      using NodePtr = std::shared_ptr<Node>;

    private:
      NodePtr m_rootNode;
  };
}
}

#endif // _MCW_CAMERAWORK_CAMERA_RIG_