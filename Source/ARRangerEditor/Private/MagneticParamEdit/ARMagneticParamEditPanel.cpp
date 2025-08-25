// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticParamEdit/ARMagneticParamEditPanel.h"

#include "MagneticParamEdit/ARDetailsView.h"
#include "Components/DetailsView.h"
#include "ARMagneticParamEditHelper.h"
#include "EditorUtilityWidgetComponents.h"

#include "MagneticParamAssetPathSave.h"
#include "MagneticParamAssetSavePathFactory.h"
#include "ARRepositoryAssetHelper.h"

#include "Repositories/ARRepositoryManager.h"
#include "RepositoryAssetMaster/MagneticParametersList.h"
#include "Internal/Physics/MagneticParametersRepository.h"
#include "GameplayFramework/ARGameInstance.h"
#include "GameplayFramework/ARRepositoryAutoRegisterSystem.h"

#include "Internal/ARLoggingHeader.h"

// Slate window
#include "SMagneticParamEditPanelWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Engine/Engine.h"

using ARRanger::Physics::FMagneticParameterDTO;
using ARRanger::Editor::FARMagneticParamEditHelper;

namespace 
{
  FString ConvertAbsolutePathToObjectPath(const FString& InPath);
  FString ConvertLongPackageNameToObjectPath(const FString& InPackageName);

  bool g_bIsRegisteredPIEDelegates = false;
  FDelegateHandle gBeginPIEDelegateHandle{};
  FDelegateHandle gEndPIEDelegateHandle{};
}

void UARMagneticParamEditPanel::NativePreConstruct()
{
  Super::NativePreConstruct();

  // Add delegates when this EUW creates outside PIE
  {
    if ((GEditor != nullptr) && !GEditor->IsPlayingSessionInEditor())
    {
      if (!g_bIsRegisteredPIEDelegates)
      {
        gBeginPIEDelegateHandle = FEditorDelegates::PostPIEStarted.AddUObject(this, &UARMagneticParamEditPanel::OnBeginPIE);
        gEndPIEDelegateHandle = FEditorDelegates::EndPIE.AddUObject(this, &UARMagneticParamEditPanel::OnEndPIE);
  
        g_bIsRegisteredPIEDelegates = true;
      }
    }
  }

  if (TargetAssetPathDetailsView != nullptr)
  {
    TargetAssetPathDetailsView->SetObject(this);

    // NativePreConstruct may called multiply times
    // Use AddUniqueDynamic to avoid assertion
    TargetAssetPathDetailsView->OnPropertyChanged_Advanced.AddUniqueDynamic(this, &UARMagneticParamEditPanel::OnTargetFilePathPropertyChanged);
  }

  // TODO Temporary code
  if (TestButton != nullptr)
  {
    TestButton->OnClicked.AddUniqueDynamic(this, &UARMagneticParamEditPanel::OnAssetPathSaved);
  }

}

void UARMagneticParamEditPanel::NativeConstruct()
{
  Super::NativeConstruct();

  ConnectToRepository();
  CreateSlatePanel();
}

void UARMagneticParamEditPanel::NativeDestruct()
{
  // Remove delegates if registered
  {
    if (g_bIsRegisteredPIEDelegates)
    {
      FEditorDelegates::PostPIEStarted.Remove(gBeginPIEDelegateHandle);
      FEditorDelegates::EndPIE.Remove(gEndPIEDelegateHandle);
      
      gBeginPIEDelegateHandle.Reset();
      gEndPIEDelegateHandle.Reset();
      g_bIsRegisteredPIEDelegates = false;
    }
  }

  if (TargetAssetPathDetailsView != nullptr)
  {
    TargetAssetPathDetailsView->OnPropertyChanged_Advanced.RemoveDynamic(this, &UARMagneticParamEditPanel::OnTargetFilePathPropertyChanged);
  }

  if (TestButton != nullptr)
  {
    TestButton->OnClicked.RemoveDynamic(this, &UARMagneticParamEditPanel::OnAssetPathSaved);
  }

  RemoveSlatePanel();
  DisconnectToRepository();
  
  Super::NativeDestruct();
}

void UARMagneticParamEditPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
  Super::NativeTick(MyGeometry, InDeltaTime);
}

void UARMagneticParamEditPanel::ConnectToRepository()
{
  if (m_repositoryConnection != nullptr)
  {
    return;
  }

  using ARRanger::Repository::FARRepositoryManager;
  using ARRanger::Physics::FARPhysicsMagneticParametersRepository;

  m_repositoryConnection = FARRepositoryManager::Get().GetRepository<FARPhysicsMagneticParametersRepository>();

}

void UARMagneticParamEditPanel::DisconnectToRepository()
{
  m_repositoryConnection = nullptr;
}

bool UARMagneticParamEditPanel::IsConnectedToRepository() const
{
  return m_repositoryConnection != nullptr;
}

void UARMagneticParamEditPanel::CreateSlatePanel()
{
  if (OnScreenPanelWidget.IsValid())
  {
    return;
  }

  if (TryInitRepository())
  {
    UE_LOG(LogTemp, Warning, TEXT("Repository is not initialized.Please check FilePath"));
  }

  // Create In GameViewport
  if ((GEngine != nullptr) && (GEngine->GameViewport != nullptr))
  {
    OnScreenPanelWidget = SNew(SMagneticParamEditPanelWidget).RepositoryConnection(m_repositoryConnection);
    GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(OnScreenPanelWidgetContainer, SWeakWidget).PossiblyNullContent(OnScreenPanelWidget.ToSharedRef()));
  }
  
}

void UARMagneticParamEditPanel::RemoveSlatePanel()
{
  if ((GEngine != nullptr) && 
      (GEngine->GameViewport != nullptr) && 
      OnScreenPanelWidgetContainer.IsValid()
     )
  {
    GEngine->GameViewport->RemoveViewportWidgetContent(OnScreenPanelWidgetContainer.ToSharedRef());
  }

  if (OnScreenPanelWidget.IsValid())
  {
    OnScreenPanelWidget->ResetRepository();
  }
}

bool UARMagneticParamEditPanel::TryInitRepository()
{
  if (m_repositoryConnection == nullptr)
  {
    return false;
  }

  UObject* assetObj = UARRepositoryAssetHelper::GetAssetByClass
                    (
                      TargetAssetPath.FilePath,               // Long package name
                      UMagneticParametersList::StaticClass(),
                      nullptr,
                      false           // Find asset without creating one
                    );  
  
  if (assetObj == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("Load failed. File path: [%s]"), *TargetAssetPath.FilePath);
    return false;
  }

  if (UMagneticParametersList* actualAsset = Cast<UMagneticParametersList>(assetObj))
  {
    for (const auto& [ name, dataEntry ] : actualAsset->MagneticParameters)
    {
      ARRanger::Physics::FMagneticParameterDTO dto = FMagneticParameterDTO {dataEntry.MagneticCharge, dataEntry.MegneticObjectMass};

      if (!m_repositoryConnection->Exist(dataEntry.MagneticPhysicsUserClass->GetDefaultObject()))
      {
        m_repositoryConnection->Add(dataEntry.MagneticPhysicsUserClass->GetDefaultObject(), dto);
      }
      else
      {
        m_repositoryConnection->Save(dataEntry.MagneticPhysicsUserClass->GetDefaultObject(), dto);
      }
    }

    return true;
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Asset type cast failed.Current asset type:[%s]"), *assetObj->GetClass()->GetName());
  }

  return false;
}


void UARMagneticParamEditPanel::OnTargetFilePathPropertyChanged(FName PropertyName)
{
  UE_LOG(LogTemp, Error, TEXT("%s"), *PropertyName.ToString());

  // Reset File path if FilePath property of TargetAssetPath has changed
  if (PropertyName == GET_MEMBER_NAME_CHECKED(UARMagneticParamEditPanel, TargetAssetPath))
  {

    // Convert to engine relative path
    TargetAssetPath.FilePath = FPaths::ConvertRelativePathToFull(TargetAssetPath.FilePath);
    TargetAssetPath.FilePath = FPackageName::FilenameToLongPackageName(TargetAssetPath.FilePath);
  }

}

void UARMagneticParamEditPanel::OnAssetPathSaved()
{
  UFactory* assetPathSaveFactory = NewObject<UMagneticParamAssetSavePathFactory>();

  const FString saveFileLongPackageName = FPackageName::FilenameToLongPackageName(FARMagneticParamEditHelper::MagneticParamAssetPathSaveFileAbsolutePath);
  UObject* assetObj = UARRepositoryAssetHelper::GetAssetByClass
                    (
                      saveFileLongPackageName,
                      UMagneticParamAssetPathSave::StaticClass(),
                      assetPathSaveFactory
                    );
  
  if (assetObj == nullptr)
  {
    UE_LOG(LogTemp, Error, TEXT("Save failed"));
    return;
  }
  
  if (UMagneticParamAssetPathSave* actualAsset = Cast<UMagneticParamAssetPathSave>(assetObj))
  {
    // FilePath was saved as long package name before
    actualAsset->Save(ConvertLongPackageNameToObjectPath(TargetAssetPath.FilePath));

    // Save asset after modifying file
    UARRepositoryAssetHelper::SaveMagneticParametersAsset(saveFileLongPackageName);
  }
  
}

void UARMagneticParamEditPanel::OnBeginPIE(const bool bIsSimulating)
{
  CreateSlatePanel();
}

void UARMagneticParamEditPanel::OnEndPIE(const bool bIsSimulating)
{
  RemoveSlatePanel();
}

namespace 
{
  FString ConvertAbsolutePathToObjectPath(const FString& InPath)
  {
    FString relativePath = InPath;

    const FString contentDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
    if (InPath.StartsWith(contentDir))
    {
      relativePath.Reset();

      const FString relativePathWithExt = InPath.RightChop(contentDir.Len());
      const FString baseFileName = FPaths::GetBaseFilename(InPath);
      // Remove extension
      relativePath.Append(FPaths::ChangeExtension(relativePathWithExt, TEXT("")));
      relativePath = FString::Printf(TEXT("/Game/%s.%s"), *relativePath, *baseFileName);
    }

    return relativePath;
  }

  FString ConvertLongPackageNameToObjectPath(const FString& InPackageName)
  {
    if (FPackageName::IsValidLongPackageName(InPackageName))
    {
      const FString absolutePath = FPaths::ConvertRelativePathToFull(FPackageName::LongPackageNameToFilename(InPackageName));
      return ConvertAbsolutePathToObjectPath(absolutePath);
    }
    
    return FString{};
  }
}