#pragma once

#ifndef _CRP_MANGA_EXAGGERATION_STYLE_SV_EXTENSION_
#define _CRP_MANGA_EXAGGERATION_STYLE_SV_EXTENSION_

#include "SceneViewExtension.h"

namespace AR
{
namespace CustomRP
{
  class FMangaExaggerationStyleSVExtension : public FSceneViewExtensionBase
  {
    public:
      FMangaExaggerationStyleSVExtension(const FAutoRegister& AutoRegister);

      // NOTE: なるべく評価を遅らせる
      virtual int32 GetPriority() const override { return -99999; }

      
      
  }
}
}

#endif // _CRP_MANGA_EXAGGERATION_STYLE_SV_EXTENSION_
