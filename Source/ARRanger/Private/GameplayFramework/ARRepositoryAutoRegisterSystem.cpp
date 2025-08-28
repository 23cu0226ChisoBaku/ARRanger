// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFramework/ARRepositoryAutoRegisterSystem.h"
#include "GameplayFramework/ARGameInstance.h"

#include "Repositories/ARRepositoryManager.h"
#include "RepositoryAssetMaster/MagneticParametersList.h"
#include "Physics/DataTransfer/MagneticParameterDTO.h"

#include "Internal/Physics/MagneticParametersRepository.h"
#include "Internal/ARLoggingHeader.h"

using ARRanger::Repository::FARRepositoryManager;
using ARRanger::Physics::FARPhysicsMagneticParametersRepository;
using ARRanger::Physics::FMagneticParameterDTO;

void UARRepositoryAutoRegisterSystem::Initialize(FSubsystemCollectionBase& Collection)
{
  Super::Initialize(Collection);

  if (UARGameInstance* gameInstance = Cast<UARGameInstance>(GetGameInstance()))
  {
    // Initialize MagneticParam Repository
    if (!gameInstance->MagneticParamsAsset.IsNull())
    { 
      FARPhysicsMagneticParametersRepository* magRepo = nullptr;
      if (FARRepositoryManager::Get().HasRepositoryRegistered(FARPhysicsMagneticParametersRepository::GetStaticRepositoryName()))
      {
        magRepo = FARRepositoryManager::Get().GetRepository<FARPhysicsMagneticParametersRepository>();
      }
      else
      {
        magRepo = FARRepositoryManager::Get().RegisterRepository<FARPhysicsMagneticParametersRepository>();
      }

      if (magRepo != nullptr)
      {
        UMagneticParametersList* magneticAsset = gameInstance->MagneticParamsAsset.LoadSynchronous();
        for (const auto& [ dataName , data ] : magneticAsset->MagneticParameters)
        {
          if (data.MagneticPhysicsUserClass == nullptr)
          {
            continue;
          }

          UE_LOG(LogTemp, Warning, TEXT("Loading mag data : Name[%s]"), *dataName);
          FMagneticParameterDTO dto = 
          {
            .MagneticCharge = data.MagneticCharge,
            .MagneticObjectMass = data.MegneticObjectMass
          };


          if (!magRepo->Exist(data.MagneticPhysicsUserClass->GetDefaultObject()))
          {
            const bool bAddSuccess = magRepo->Add(data.MagneticPhysicsUserClass->GetDefaultObject(), dto);
            if (!bAddSuccess)
            {
              UE_LOG(LogTemp, Error, TEXT("Data add failed. [%s]"), *dataName);
            }
          }
          else
          {
            const bool bSaveSuccess = magRepo->Save(data.MagneticPhysicsUserClass->GetDefaultObject(), dto);
            if (!bSaveSuccess)
            {
              UE_LOG(LogTemp, Error, TEXT("Data save failed. [%s]"), *dataName);
            }
          }
          
        }
      }

#if WITH_EDITORONLY_DATA
      if (OnRepositoryRegistered.IsBound())
      {
        OnRepositoryRegistered.Broadcast();
      }
#endif
    }
  }
}

void UARRepositoryAutoRegisterSystem::Deinitialize()
{
  FARRepositoryManager::Get().UnregisterAll();

#if WITH_EDITORONLY_DATA
      if (OnRepositoryUnregistered.IsBound())
      {
        OnRepositoryUnregistered.Broadcast();
      }
#endif
}