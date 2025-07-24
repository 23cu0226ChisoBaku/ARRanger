// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#ifndef _MOTION_DIFF_CONCEPTS_
#define _MOTION_DIFF_CONCEPTS_

#include <type_traits>

class FMDMeshCaptureProxy;

namespace MotionDiff
{
  namespace Concepts
  {
    template<typename MeshCaptureProxyType>
    concept MeshCaptureProxyConcept = requires
    {
      requires std::is_base_of_v<FMDMeshCaptureProxy ,MeshCaptureProxyType>;
    };
  }
}

#endif 