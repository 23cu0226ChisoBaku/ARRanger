#include "PlayerObservation/ObserverListRootNode.h"
#include "PlayerObservation/ObserverListNode.h"

namespace ARRanger
{
  FObserverListRootNode::FObserverListRootNode()
    : m_listRootNode{nullptr}
  {

  }

  FObserverListRootNode::~FObserverListRootNode()
  {
    Reset();
  }

  TSharedPtr<FObserverListNode> FObserverListRootNode::MakeListNode()
  {
    FObserverListNode* newListNodePtr = new FObserverListNode();
    return TSharedPtr<FObserverListNode>{newListNodePtr};
  }

  void FObserverListRootNode::AttachNewObserverList(const TSharedPtr<FObserverListNode>& NewListNode)
  {
    if (m_listRootNode == nullptr)
    {
      m_listRootNode = NewListNode;
      return;
    }

    FObserverListNode* tailNode = m_listRootNode.Get();
    while (tailNode->GetNextListNode() != nullptr)
    {
      tailNode = tailNode->GetNextListNode();
    }

    tailNode->SetNextListNode(NewListNode);
  }

  void FObserverListRootNode::Reset()
  {
    FObserverListNode* tail = m_listRootNode.Get();

    while (tail != nullptr)
    {
      FObserverListNode* temp = tail;
      tail = tail->GetNextListNode();

      temp->Reset();
    }
  }

  void FObserverListRootNode::Evaluate()
  {
    if (m_listRootNode == nullptr)
    {
      return;
    }

    m_listRootNode->Evaluate();
  }

}