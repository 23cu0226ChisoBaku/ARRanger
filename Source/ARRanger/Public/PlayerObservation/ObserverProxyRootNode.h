#pragma once

#ifndef _AR_OBSERVER_PROXY_ROOT_NODE_
#define _AR_OBSERVER_PROXY_ROOT_NODE_

namespace ARRanger
{
  class FObserverProxyNode;

  // TODO Can make this as a devired class of FObserverProxyNode
  class FObserverProxyRootNode
  {
    public:
      FObserverProxyRootNode();
      ~FObserverProxyRootNode() = default;

      static TSharedPtr<FObserverProxyNode> MakeNode(const TDelegate<void()>& NodeEvent);

      void PushNodeBack(const TSharedPtr<FObserverProxyNode>& Node);
      void PushNodeFront(const TSharedPtr<FObserverProxyNode>& Node);
      void RemoveNodeBack();
      void RemoveNodeFront();
      void RemoveAll();
      void Evaluate();

    private:
      TSharedPtr<FObserverProxyNode> m_rootNode = nullptr;  
  };
}

#endif // _AR_OBSERVER_PROXY_ROOT_NODE_