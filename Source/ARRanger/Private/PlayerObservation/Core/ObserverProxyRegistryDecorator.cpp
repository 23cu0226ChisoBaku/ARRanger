// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerObservation/Core/ObserverProxyRegistryDecorator.h"

namespace ARRanger
{

FObserverProxyRegistryDecorator::FObserverProxyRegistryDecorator()
  : m_registry{nullptr}
{ }

FObserverProxyRegistryDecorator::FObserverProxyRegistryDecorator(const TSharedPtr<IObserverRegistry>& Registry)
  : m_registry{Registry}
{ }

FObserverProxyRegistryDecorator::~FObserverProxyRegistryDecorator() = default;

void FObserverProxyRegistryDecorator::RegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers)
{
  OnRegisterObserverProxy(Observers);

  if (m_registry.IsValid())
  {
    m_registry->RegisterObserverProxy(Observers);
  }
}

void FObserverProxyRegistryDecorator::OnRegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers)
{
  // Empty implementation
}

} // namespace ARRanger
