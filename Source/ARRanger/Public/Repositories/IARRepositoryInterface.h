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
  struct IARRepositoryInterface
  {
    virtual ~IARRepositoryInterface() = default;
    ARRANGER_API virtual FString GetRepositoryName() const = 0;
  }; 
}

}

#endif // _AR_REPOSITORIES_INTERFACE_