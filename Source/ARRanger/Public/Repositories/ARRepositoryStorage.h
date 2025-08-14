#pragma once

#ifndef _AR_REPOSITORIES_STORAGE_
#define _AR_REPOSITORIES_STORAGE_

#include <type_traits>

namespace ARRanger
{

namespace Repository
{
  struct IARRepositoryInterface;

  struct FARRepositoryStorage
  {
    public:
      FARRepositoryStorage(const FARRepositoryStorage&) = delete;
      FARRepositoryStorage(FARRepositoryStorage&&) = delete;
      FARRepositoryStorage& operator=(const FARRepositoryStorage&) = delete;
      FARRepositoryStorage& operator=(FARRepositoryStorage&&) = delete;

    public:
      ARRANGER_API static FARRepositoryStorage& GetStorage();

      ARRANGER_API void RegisterRepository(IARRepositoryInterface* RepositoryInterface);

      template<typename RepositoryType>
      RepositoryType* GetRepository() const;
    
    private:
      FARRepositoryStorage();
      ~FARRepositoryStorage();

      ARRANGER_API IARRepositoryInterface* GetRepository_Internal(FString RepositoryName);

      TMap<FString, IARRepositoryInterface*> m_repositoryMap;
  };

  template<typename RepositoryType>
  RepositoryType* FARRepositoryStorage::GetRepository() const
  {
    static_assert(std::is_base_of_v<RepositoryType, IARRepositoryInterface>, "Invalid repository type");

    return static_cast<RepositoryType*>(GetRepository_Internal(RepositoryType::GetStaticRepositoryName()));
  }
}

}

#endif // _AR_REPOSITORIES_STORAGE_