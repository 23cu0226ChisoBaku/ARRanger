#pragma once

#ifndef _AR_PHYSICS_DATA_TRANSFER_MAGENETIC_GATEWAY_
#define _AR_PHYSICS_DATA_TRANSFER_MAGENETIC_GATEWAY_

#include "Repositories/IARRepositoryInterface.h"

class UObject;

namespace ARRanger
{

namespace Physics
{
  struct FMagneticParameterDTO
  {
    ARRANGER_API static const FMagneticParameterDTO EmptyDTO;

    /**磁力オブジェクト磁極の強さ */
    float MagneticCharge = 0.0f;

    /**磁力オブジェクトの重さ */
    float MagneticObjectMass = 0.0f; 
  };

  struct IMagneticGatewayInterface : public ARRanger::Repository::IARRepositoryInterface
  {
    ARRANGER_API virtual ~IMagneticGatewayInterface() = default;

    ARRANGER_API virtual [[nodiscard]] bool Find(UObject* User, FMagneticParameterDTO& OutData) const = 0;

    ARRANGER_API virtual [[nodiscard]] int32 FindAll(TArray<FMagneticParameterDTO>& OutAllDatas) const = 0;

    ARRANGER_API virtual [[nodiscard]] bool Save(UObject* User, const FMagneticParameterDTO& InData) = 0;
  };

} // namespace ARRanger::Physics

} // namespace ARRanger

#endif // _AR_PHYSICS_DATA_TRANSFER_MAGENETIC_GATEWAY_