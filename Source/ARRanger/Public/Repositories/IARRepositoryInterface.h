#pragma once

#ifndef _AR_REPOSITORIES_INTERFACE_
#define _AR_REPOSITORIES_INTERFACE_

#define AR_DEFINE_REPOSITORY_CLASS(ClassType) \
  static FString GetStaticRepositoryName() { return TEXT(#ClassType); } \
  FString GetRepositoryName() const override { return GetStaticRepositoryName(); }

namespace ARRanger
{

namespace Repository
{
  class IARRepositoryInterface
  {
    public:
      virtual ~IARRepositoryInterface() = default;
      ARRANGER_API virtual FString GetRepositoryName() const = 0;
  };
 
  template<typename RepositoryIDType, typename DataType>
  class IARRepositoryGateway : public ARRanger::Repository::IARRepositoryInterface
  {
    public:
      virtual ~IARRepositoryGateway() = default;

      ARRANGER_API virtual [[nodiscard]] bool Find(RepositoryIDType ID, DataType& OutData) const = 0;

      ARRANGER_API virtual [[nodiscard]] int32 FindAll(TArray<DataType>& OutAllDatas) const = 0;

      ARRANGER_API virtual [[nodiscard]] bool Add(RepositoryIDType ID, const DataType& InData) = 0;
      
      ARRANGER_API virtual [[nodiscard]] bool Save(RepositoryIDType ID, const DataType& InData) = 0;

      ARRANGER_API virtual [[nodiscard]] bool Delete(RepositoryIDType ID) = 0;

      ARRANGER_API virtual [[nodiscard]] bool Exist(RepositoryIDType ID) const = 0;
  };

}

}

#endif // _AR_REPOSITORIES_INTERFACE_