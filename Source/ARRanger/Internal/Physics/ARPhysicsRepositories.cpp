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

bool FARPhysicsMagneticParametersRepository::Find(UObject* ID, FMagneticParameterDTO& OutData) const
{
  OutData = FMagneticParameterDTO::EmptyDTO;

  if (ID == nullptr)
  {
    AR_LOG(LogARRepository, Error, TEXT("User is invalid."));
    return false;
  }

  TSubclassOf<UObject> userClass = ID->GetClass();
  if (!m_container.Contains(userClass))
  {
    AR_LOG(LogARRepository, Error, TEXT("Can not find data of User. User Class Name:[%s]"), *ID->GetClass()->GetName());
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

bool FARPhysicsMagneticParametersRepository::Add(UObject* ID, const FMagneticParameterDTO& InData)
{
  if (ID == nullptr)
  {
    AR_LOG(LogARRepository, Error, TEXT("Can not add invalid user into repository."));
    return false;
  }

  TSubclassOf<UObject> userClass = ID->GetClass();
  if (m_container.Contains(userClass))
  {
    AR_LOG(LogARRepository, Warning, TEXT("Try to add same user into repository"));
    return false;
  }

  (void)m_container.Emplace(userClass, FMagneticParameters{InData.MagneticCharge, InData.MagneticObjectMass});
  return true;
}


bool FARPhysicsMagneticParametersRepository::Save(UObject* ID, const FMagneticParameterDTO& InData)
{
  if (ID == nullptr)
  {
    AR_LOG(LogARRepository, Error, TEXT("Can not save invalid user into repository."));
    return false;
  }
  
  FMagneticParameters* userData = m_container.Find(ID->GetClass());
  if (userData == nullptr)
  {
    AR_LOG(LogARRepository, Error, TEXT("Try to save data that is not exist."));
    return false;
  }

  if (userData->MagneticCharge == InData.MagneticCharge &&
      userData->MagneticObjectMass == InData.MagneticObjectMass)
    {
      AR_LOG(LogARRepository, Warning, TEXT("Try to save same value.Repository would not update"));
      return false;
    }
    
    userData->MagneticCharge = InData.MagneticCharge;
    userData->MagneticObjectMass = InData.MagneticObjectMass;
    
#if WITH_EDITOR
    MarkDirty();
#endif
  
  return true;
}
  
bool FARPhysicsMagneticParametersRepository::Delete(UObject* ID)
{
  if (ID == nullptr)
  {
    return false;
  }

  return m_container.Remove(ID->GetClass()) > 0;

}

bool FARPhysicsMagneticParametersRepository::Exist(UObject* ID) const
{
  if (ID == nullptr)
  {
    return false;
  }

  return m_container.Contains(ID->GetClass());
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