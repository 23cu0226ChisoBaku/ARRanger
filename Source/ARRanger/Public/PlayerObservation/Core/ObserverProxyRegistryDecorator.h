// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayerObservation/IObserverRegistry.h"

namespace ARRanger
{
  class FObserverProxyRegistryDecorator : public IObserverRegistry
  {
    public:
      ARRANGER_API FObserverProxyRegistryDecorator();
      ARRANGER_API explicit FObserverProxyRegistryDecorator(const TSharedPtr<IObserverRegistry>& Registry);
      ARRANGER_API ~FObserverProxyRegistryDecorator();

      /**Start of IObserverRegistry interface */
      ARRANGER_API void RegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers) override final; 

    protected:
      ARRANGER_API virtual void OnRegisterObserverProxy(const TSharedPtr<FObserverListNode>& Observers);

    private:
      TSharedPtr<IObserverRegistry> m_registry;
  };
}
