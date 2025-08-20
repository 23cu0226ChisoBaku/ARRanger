#include "Repositories/ARRepositoryManager.h"

#include "Repositories/IARRepositoryInterface.h"
#include "Internal/ARLoggingHeader.h"

namespace ARRanger
{

namespace Repository
{
  FARRepositoryManager& FARRepositoryManager::Get()
  {
    static FARRepositoryManager s_instance;
    return s_instance;
  }

  void FARRepositoryManager::UnregisterAll()
  {
    TArray<FString> keys;
    int32 pairNum = m_repositoryMap.GetKeys(keys);
    for (int32 idx = 0; idx < pairNum; ++idx)
    {
      UnregisterRepository_Internal(keys[idx]);
    }
  }

  bool FARRepositoryManager::HasRepositoryRegistered(const FString& RepositoryName) const
  {
    return m_repositoryMap.Contains(RepositoryName);
  }

  FARRepositoryManager::FARRepositoryManager()
    : m_repositoryMap{}
  { }

  FARRepositoryManager::~FARRepositoryManager()
  {
    UnregisterAll();
    m_repositoryMap.Reset();
  }
  
  IARRepositoryInterface* FARRepositoryManager::GetRepository_Internal(const FString& RepositoryName)
  {
    if (!m_repositoryMap.Contains(RepositoryName))
    {
      return nullptr;
    }

    return m_repositoryMap[RepositoryName];
  }
    
  void FARRepositoryManager::RegisterRepository_Internal(IARRepositoryInterface* RepositoryInterface)
  {
    check(RepositoryInterface != nullptr);
    const FString repositoryName = RepositoryInterface->GetRepositoryName();

    if (m_repositoryMap.Contains(repositoryName))
    {
      AR_LOG(LogARRepository, Warning, TEXT("Repository is already registered. Repository name:[%s]"), *repositoryName);
      return;
    }

    m_repositoryMap.Emplace(repositoryName, RepositoryInterface);
  }

  bool FARRepositoryManager::UnregisterRepository_Internal(const FString& RepositoryName)
  {
    if (HasRepositoryRegistered(RepositoryName))
    {
      IARRepositoryInterface* temp = m_repositoryMap[RepositoryName];
      (void)m_repositoryMap.Remove(RepositoryName);

#if WITH_EDITOR
      UE_LOG(LogTemp, Warning, TEXT("Unregister repository : [%s]"), *RepositoryName);
#endif
      // TODO Move memory allocation to Allocator class.
      delete temp;
      return true;
    }

    return false;
  }
}

}
