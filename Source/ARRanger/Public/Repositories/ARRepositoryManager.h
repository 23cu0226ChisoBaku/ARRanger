#pragma once

#ifndef _AR_REPOSITORIES_STORAGE_
#define _AR_REPOSITORIES_STORAGE_

#include <type_traits>

namespace ARRanger
{

namespace Repository
{
  class IARRepositoryInterface;

  class FARRepositoryManager
  {
    public:
      ARRANGER_API static FARRepositoryManager& Get();

      template<typename RepositoryType>
      RepositoryType* GetRepository();

      template<typename RepositoryType>
      RepositoryType* RegisterRepository();

      template<typename RepositoryType>
      bool UnregisterRepository();

      ARRANGER_API void UnregisterAll();

      ARRANGER_API bool HasRepositoryRegistered(const FString& RepositoryName) const;
    
    private:
      FARRepositoryManager();
      ~FARRepositoryManager();

      ARRANGER_API IARRepositoryInterface* GetRepository_Internal(const FString& RepositoryName);
      ARRANGER_API void RegisterRepository_Internal(IARRepositoryInterface* RepositoryInterface);
      ARRANGER_API bool UnregisterRepository_Internal(const FString& RepositoryName);

      TMap<FString, IARRepositoryInterface*> m_repositoryMap;

    public:
      FARRepositoryManager(const FARRepositoryManager&) = delete;
      FARRepositoryManager(FARRepositoryManager&&) = delete;
      FARRepositoryManager& operator=(const FARRepositoryManager&) = delete;
      FARRepositoryManager& operator=(FARRepositoryManager&&) = delete;
  };

  template<typename RepositoryType>
  RepositoryType* FARRepositoryManager::GetRepository()
  {
    static_assert(std::is_base_of_v<IARRepositoryInterface, RepositoryType>, "Invalid repository type");
    static_assert(sizeof(RepositoryType) > 0, "Incomplete type of repository");

    return static_cast<RepositoryType*>(GetRepository_Internal(RepositoryType::GetStaticRepositoryName()));
  }

  template<typename RepositoryType>
  RepositoryType* FARRepositoryManager::RegisterRepository()
  {
    static_assert(std::is_base_of_v<IARRepositoryInterface, RepositoryType>, "Invalid repository type");
    static_assert(sizeof(RepositoryType) > 0, "Incomplete type of repository");

    if (HasRepositoryRegistered(RepositoryType::GetStaticRepositoryName()))
    {
      return GetRepository<RepositoryType>();
    }

    // TODO Move memory allocation to Allocator class.
    RepositoryType* newRepository = new RepositoryType{};
    RegisterRepository_Internal(newRepository);

    return newRepository;
  }

  template<typename RepositoryType>
  bool FARRepositoryManager::UnregisterRepository()
  {
    static_assert(std::is_base_of_v<IARRepositoryInterface, RepositoryType>, "Invalid repository type");
    static_assert(sizeof(RepositoryType) > 0, "Incomplete type of repository");

    return UnregisterRepository_Internal(RepositoryType::GetStaticRepositoryName());
  }
}

}

#endif // _AR_REPOSITORIES_STORAGE_