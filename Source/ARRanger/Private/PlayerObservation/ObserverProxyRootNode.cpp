#include "PlayerObservation/ObserverProxyRootNode.h"

// TODO Move this to Header if FObserverProxyRootNode becomes a devired class of FObserverProxyNode
#include "PlayerObservation/ObserverProxyNode.h"

namespace ARRanger
{
  FObserverProxyRootNode::FObserverProxyRootNode()
    : m_rootNode{nullptr}
  { }

  TSharedPtr<FObserverProxyNode> FObserverProxyRootNode::MakeNode(const TDelegate<void()>& NodeEvent)
  {
    TSharedPtr<FObserverProxyNode> newNodePtr = ::MakeShared<FObserverProxyNode>(FObserverProxyNode::PrivateToken{});
    newNodePtr->InitializeNode(NodeEvent);

    return newNodePtr;
  }

  void FObserverProxyRootNode::PushNodeBack(const TSharedPtr<FObserverProxyNode>& Node)
  {
    if (m_rootNode == nullptr)
    {
      m_rootNode = Node;
      return;
    }

    FObserverProxyNode* head = m_rootNode.Get();
    while(head->GetNextNode() != nullptr)
    {
      head = head->GetNextNode();
    }

    head->SetNextNode(Node);
  }

  void FObserverProxyRootNode::PushNodeFront(const TSharedPtr<FObserverProxyNode>& Node)
  {
    if (Node != nullptr)
    {
      Node->SetNextNode(m_rootNode);

      m_rootNode = Node;
    }
  }

  void FObserverProxyRootNode::RemoveNodeBack()
  {
    if (!m_rootNode.IsValid())
    {
      return;
    }

    FObserverProxyNode* head = m_rootNode.Get();
    FObserverProxyNode* headBefore = nullptr;

    while(head->GetNextNode() != nullptr)
    {
      headBefore = head;
      head = head->GetNextNode();
    }

    // 最後尾のノードをリストから外す。 ※最後尾の一個前のノードのNextNodeをnullptrに設定
    headBefore->SetNextNode(TSharedPtr<FObserverProxyNode>{nullptr});
  }

  void FObserverProxyRootNode::RemoveNodeFront()
  {
    if (m_rootNode.IsValid())
    {
      FObserverProxyNode* nextNodeTemp = m_rootNode->GetNextNode();
      if (nextNodeTemp != nullptr)
      {
        TSharedPtr<FObserverProxyNode> tempSharedPtr = nextNodeTemp->AsShared();
        m_rootNode = tempSharedPtr;
      }
      else
      {
        m_rootNode.Reset();
      }
    }
  }

  void FObserverProxyRootNode::RemoveAll()
  {
    while (m_rootNode.IsValid())
    {
      RemoveNodeFront();
    }
  }

  void FObserverProxyRootNode::Evaluate()
  {
    if (!m_rootNode.IsValid())
    {
      return;
    }

    m_rootNode->Evaluate();
  }
}