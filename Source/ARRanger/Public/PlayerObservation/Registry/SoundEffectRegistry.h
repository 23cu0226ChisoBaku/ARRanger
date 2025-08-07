#pragma once

#ifndef _AR_PLAYER_OBSERVATION_REGISTRY_SOUND_EFFECT_
#define _AR_PLAYER_OBSERVATION_REGISTRY_SOUND_EFFECT_

#include "PlayerObservation/Core/ObserverProxyRegistryDecorator.h"

struct FARSoundEffectParameters;

namespace ARRanger
{
  class FSoundEffectRegistry : public FObserverProxyRegistryDecorator
  {
    using Super = FObserverProxyRegistryDecorator;

    public:
      ARRANGER_API explicit FSoundEffectRegistry(const FARSoundEffectParameters& Param, const TSharedPtr<IObserverRegistry>& Registry = nullptr);
      ARRANGER_API ~FSoundEffectRegistry();

    protected:
      ARRANGER_API virtual void OnRegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers);

    private:
      TSharedPtr<const FARSoundEffectParameters> m_seParamPtr;
  };
}

#endif // _AR_PLAYER_OBSERVATION_REGISTRY_SOUND_EFFECT_