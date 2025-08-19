#pragma once

#ifndef _AR_PHYSICS_DATA_TRANSFER_MAGENETIC_PARAMETER_DTO_
#define _AR_PHYSICS_DATA_TRANSFER_MAGENETIC_PARAMETER_DTO_

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

} // namespace ARRanger::Physics

} // namespace ARRanger

#endif // _AR_PHYSICS_DATA_TRANSFER_MAGENETIC_PARAMETER_DTO_