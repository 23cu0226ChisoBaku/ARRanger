#include "ARPhysicsModels.h"
#include "MagneticParametersRepository.h"

#include "Internal/ARLoggingHeader.h"

#include <type_traits>

namespace ARRanger
{

namespace Physics
{

FMagneticParameterDTO const FMagneticParameterDTO::EmptyDTO = {};

FARPhysicsMagneticParametersRepository::FARPhysicsMagneticParametersRepository()
  : m_container{}
#if WITH_EDITORONLY_DATA
  , bIsRepositoryModified{false}
#endif
{ }

FARPhysicsMagneticParametersRepository::~FARPhysicsMagneticParametersRepository()
{
  Reset();
}

bool FARPhysicsMagneticParametersRepository::Find(UObject* User, FMagneticParameterDTO& OutData) const
{
  OutData = FMagneticParameterDTO::EmptyDTO;

  if (User == nullptr)
  {
    AR_LOG(LogARPhysicsRepository, Error, TEXT("User is invalid."));
    return false;
  }

  TSubclassOf<UObject> userClass = User->GetClass();
  if (!m_container.Contains(userClass))
  {
    AR_LOG(LogARPhysicsRepository, Error, TEXT("Can not find data of User. User Class Name:[%s]"), *User->GetClass()->GetName());
    return false;
  } 

  OutData.MagneticCharge = m_container[userClass].MagneticCharge;
  OutData.MagneticObjectMass = m_container[userClass].MagneticObjectMass;

  return true;
}

int32 FARPhysicsMagneticParametersRepository::FindAll(TArray<FMagneticParameterDTO>& OutAllDatas) const
{
  OutAllDatas.Reset(m_container.Num());

  for (const auto& [ _ , data] : m_container)
  {
    OutAllDatas.Emplace(FMagneticParameterDTO{data.MagneticCharge, data.MagneticObjectMass});
  }

  return OutAllDatas.Num();
} 

bool FARPhysicsMagneticParametersRepository::Save(UObject* User, const FMagneticParameterDTO& InData)
{
  if (User == nullptr)
  {
    AR_LOG(LogARPhysicsRepository, Error, TEXT("Can not save invalid user into repository."));
    return false;
  }

  FMagneticParameters& userData = m_container.FindOrAdd(User->GetClass());
  if (userData.MagneticCharge == InData.MagneticCharge &&
      userData.MagneticObjectMass == InData.MagneticObjectMass)
  {
    AR_LOG(LogARPhysicsRepository, Warning, TEXT("Try to save same value.Repository would not update"));
    return false;
  }

  userData.MagneticCharge = InData.MagneticCharge;
  userData.MagneticObjectMass = InData.MagneticObjectMass;

#if WITH_EDITOR
  MarkDirty();
#endif

  return true;
}

void FARPhysicsMagneticParametersRepository::Reset()
{
  m_container.Reset();
}

#if WITH_EDITOR
void FARPhysicsMagneticParametersRepository::MarkDirty()
{
  #if WITH_EDITORONLY_DATA
    bIsRepositoryModified = true;
  #endif
}

bool FARPhysicsMagneticParametersRepository::IsRepositoryModified() const
{
  return bIsRepositoryModified;
}
#endif

} 

}