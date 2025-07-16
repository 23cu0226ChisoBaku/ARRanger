#pragma once

#ifndef _CRP_MANGA_EXAGGERATION_STYLE_SV_EXTENSION_
#define _CRP_MANGA_EXAGGERATION_STYLE_SV_EXTENSION_

#include "SceneViewExtension.h"

class UCustomRPWorldSubsystem;

namespace AR
{
namespace CustomRP
{
  class FMangaExaggerationStyleSVExtension : public FSceneViewExtensionBase
  {
    public:
      FMangaExaggerationStyleSVExtension(const FAutoRegister& AutoRegister, UCustomRPWorldSubsystem* Subsystem);

      virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;

      // NOTE: なるべく評価を遅らせる
      virtual int32 GetPriority() const override { return -99999; }

      void Invalidate();

    private:

      UCustomRPWorldSubsystem* m_CRPSubsystem;

  };
}
}

#endif // _CRP_MANGA_EXAGGERATION_STYLE_SV_EXTENSION_
