#pragma once

#ifndef _AR_INTERNAL_PHYSICS_MAGNETIC_PARAMETERS_REPOSITORY_
#define _AR_INTERNAL_PHYSICS_MAGNETIC_PARAMETERS_REPOSITORY_

#include "Physics/DataTransfer/IMagneticGatewayInterface.h"

namespace ARRanger
{

namespace Physics
{
  struct FMagneticParameters;

  class FARPhysicsMagneticParametersRepository final : public ARRanger::Physics::IMagneticGatewayInterface
  {
    using RepositoryContainer = TMap<TSubclassOf<UObject>, FMagneticParameters>;
    
    public:
      AR_DEFINE_REPOSITORY_CLASS(FARPhysicsMagneticParametersRepository)

    public:
      FARPhysicsMagneticParametersRepository();
      ~FARPhysicsMagneticParametersRepository();

      /**Start IMagneticGatewayInterface interface */
      [[nodiscard]] bool Find(UObject* User, FMagneticParameterDTO& OutData) const override;
      [[nodiscard]] int32 FindAll(TArray<FMagneticParameterDTO>& OutAllDatas) const override;
      [[nodiscard]] bool Save(UObject* User, const FMagneticParameterDTO& InData) override; 
      /**End IMagneticGatewayInterface interface */

      FARPhysicsMagneticParametersRepository(const FARPhysicsMagneticParametersRepository& Other) = delete;
      FARPhysicsMagneticParametersRepository& operator=(const FARPhysicsMagneticParametersRepository& Other) = delete;

    private:
      void Reset();
    
    private:
      RepositoryContainer m_container;

// Editor only
#pragma region EDITOR_ONLY_REGION
  #if WITH_EDITOR
    private:
      void MarkDirty();

    public:
      bool IsRepositoryModified() const;
  #endif

  #if WITH_EDITORONLY_DATA
    private:
      uint8 bIsRepositoryModified : 1;
  #endif
#pragma endregion EDITOR_ONLY_REGION
  };

} // namespace ARRanger::Physics

} // namespace ARRanger

#endif // _AR_INTERNAL_PHYSICS_MAGNETIC_PARAMETERS_REPOSITORY_