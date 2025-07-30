// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ObserverListNode.h"
#include "Core/ObserverProxyRegistryFwd.h"
#include "Core/ObserverProxyRegistryMisc.h"

namespace ARRanger
{
  class IObserverRegistry
  {
    public:
      virtual ~IObserverRegistry() = default;

      ARRANGER_API virtual void RegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers) = 0;
  };

} // namespace ARRanger

