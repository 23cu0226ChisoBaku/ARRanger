#pragma once

#ifndef _MCW_CAMERAWORK_CORE_CAMERA_WORK_DIRECTOR_
#define _MCW_CAMERAWORK_CORE_CAMERA_WORK_DIRECTOR_

#include "Core/CoreDefines.h"

namespace AR
{
namespace CameraWork
{
  class FCameraWorkDirectorEvaluator;
  using FCameraWorkDirectorEvaluatorPtr = AR::CameraWork::FCameraWorkDirectorEvaluator*;
  
  /**
   * Camera work director to direct current use camera rig
   */
  class FCameraWorkDirector
  {
    public:
      MCW_API FCameraWorkDirector();
      MCW_API virtual ~FCameraWorkDirector();

    public:
      MCW_API FCameraWorkDirectorEvaluatorPtr MakeDirectorEvaluator() const;

    protected:
      MCW_API FCameraWorkDirectorEvaluatorPtr MakeDirectorEvaluatorImpl() const;


  };
}
}
#endif // _MCW_CAMERAWORK_CORE_CAMERA_WORK_DIRECTOR_ 