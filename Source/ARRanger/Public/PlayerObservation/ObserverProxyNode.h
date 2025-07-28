#pragma once

#ifndef _AR_OBSERVER_PROXY_NODE_
#define _AR_OBSERVER_PROXY_NODE_

#include "PlayerObservation/ObserverProxy.h"

namespace ARRanger
{
  class FObserverProxyNode : public TSharedFromThis<FObserverProxyNode>
  {
    // TODO Remove this if it becomes a devired class of FObserverProxyNode
    friend class FObserverProxyRootNode;

    private:
      struct PrivateToken {};

    public:
      explicit FObserverProxyNode(PrivateToken Token, const TSharedPtr<FObserverProxyNode>& NextNodePtr = nullptr);

    public:
      virtual ~FObserverProxyNode() = default;
      
      void InitializeNode(const TDelegate<void()>& ProxyEvent);
      void SetNextNode(const TSharedPtr<FObserverProxyNode>& NextNodePtr);
      void Evaluate();

      FObserverProxyNode* GetNextNode() const { return m_next.Get(); }
      
    private:
      TSharedPtr<FObserverProxyNode> m_next = nullptr;
      FObserverProxy m_proxy;
      
    public:
      FObserverProxyNode(const FObserverProxyNode& Other) = delete;
      FObserverProxyNode& operator=(const FObserverProxyNode& Other) = delete;
      FObserverProxyNode(FObserverProxyNode&& Other) = delete;
      FObserverProxyNode& operator=(FObserverProxyNode&& Other) = delete;
  };
}

#endif // _AR_OBSERVER_PROXY_NODE_