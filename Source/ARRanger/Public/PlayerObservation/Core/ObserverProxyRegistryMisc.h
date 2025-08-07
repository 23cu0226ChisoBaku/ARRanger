#pragma once

#ifndef _AR_PLAYER_OBSERVATION_OBSERVER_PROXY_REGISTRY_MISC_
#define _AR_PLAYER_OBSERVATION_OBSERVER_PROXY_REGISTRY_MISC_

#include <type_traits>

namespace ARRanger
{
  class IObserverRegistry;
}

template<typename RegistryType>
concept RegistryConcept = requires
{
  requires std::is_base_of_v<ARRanger::IObserverRegistry, RegistryType>;
};

template<RegistryConcept RegistryType, typename... ArgTypes>
TSharedPtr<ARRanger::IObserverRegistry> MakeRegistry(ArgTypes&&... Args)
{
  return ::StaticCastSharedPtr<ARRanger::IObserverRegistry>(::MakeShared<RegistryType>(::Forward<ArgTypes>(Args)...).ToSharedPtr());
}

#endif // _AR_PLAYER_OBSERVATION_OBSERVER_PROXY_REGISTRY_MISC_