#pragma once

#ifndef _AR_INTERNAL_PHYSICS_MAGNETIC_PARAMETERS_REPOSITORY_
#define _AR_INTERNAL_PHYSICS_MAGNETIC_PARAMETERS_REPOSITORY_

#include "Repositories/IARRepositoryInterface.h"
#include "Physics/DataTransfer/MagneticParameterDTO.h"
#include "Internal/Physics/ARPhysicsModels.h"

namespace ARRanger
{

namespace Physics
{
  class FARPhysicsMagneticParametersRepository final : public ARRanger::Repository::IARRepositoryGateway<UObject*, FMagneticParameterDTO>
  {
    using RepositoryContainer = TMap<TSubclassOf<UObject>, FMagneticParameters>;
    
    public:
      /**Start IARRepositoryInterface interface */
      AR_DEFINE_REPOSITORY_CLASS(FARPhysicsMagneticParametersRepository)
      /**End IARRepositoryInterface interface */

    public:
      FARPhysicsMagneticParametersRepository();
      ~FARPhysicsMagneticParametersRepository();

      /**Start IARRepositoryGateway interface */
      ARRANGER_API [[nodiscard]] bool Find(UObject* ID, FMagneticParameterDTO& OutData) const override;
      ARRANGER_API [[nodiscard]] int32 FindAll(TArray<FMagneticParameterDTO>& OutAllDatas) const override;
      ARRANGER_API [[nodiscard]] bool Add(UObject* ID, const FMagneticParameterDTO& InData) override;
      ARRANGER_API [[nodiscard]] bool Save(UObject* ID, const FMagneticParameterDTO& InData) override; 
      ARRANGER_API [[nodiscard]] bool Delete(UObject* ID) override;
      ARRANGER_API [[nodiscard]] bool Exist(UObject* ID) const override;
      /**End IARRepositoryGateway interface */

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