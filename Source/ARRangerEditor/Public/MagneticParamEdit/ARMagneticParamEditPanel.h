// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/SoftObjectPath.h"
#include "EditorUtilityWidget.h"
#include "RepositoryAssetMaster/MagneticParametersList.h"

#include "ARMagneticParamEditPanel.generated.h"

/**Forward Declaration */
namespace ARRanger
{

namespace Repository
{
  template<typename RepositoryIDType, typename DataType>
  class IARRepositoryGateway;
} // namespace ARRanger::Repository

namespace Physics
{
  struct FMagneticParameterDTO;
} // namespace ARRanger::Physics

} // namespace ARRanger

class UARMagneticParamsListView;
class UEditorUtilityEditableText;
class UEditorUtilityButton;
// Use for self public OnPropertyChanged Delegate;
class UARDetailsView;

// Slate
class SWidget;
class SMagneticParamEditPanelWidget;


UCLASS()
class UARMagneticParamEditPanel : public UEditorUtilityWidget
{
	GENERATED_BODY()
  
  protected:
    ARRANGEREDITOR_API virtual void NativePreConstruct() override;
    ARRANGEREDITOR_API virtual void NativeConstruct() override;
    ARRANGEREDITOR_API virtual void NativeDestruct() override;
    ARRANGEREDITOR_API virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
  private:
    void ConnectToRepository();
    void DisconnectToRepository();
    bool IsConnectedToRepository() const;
    void CreateSlatePanel();
    void RemoveSlatePanel();
    bool TryInitRepository();

  private:
    UFUNCTION(Category = "AssetConnection")
    ARRANGEREDITOR_API void OnTargetFilePathPropertyChanged(FName PropertyName);

    UFUNCTION()
    ARRANGEREDITOR_API void OnAssetPathSaved();

    ARRANGEREDITOR_API void OnBeginPIE(const bool bIsSimulating);
    ARRANGEREDITOR_API void OnEndPIE(const bool bIsSimulating);

  private:
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UARDetailsView> TargetAssetPathDetailsView;
    
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UARMagneticParamsListView> MagParamListView;

    UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "AssetConnection")
    bool bAutoSaveToAsset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "AssetConnection")
    FFilePath TargetAssetPath;

    TStrongObjectPtr<UMagneticParametersList> MagneticParamAssetRef;

    FDelegateHandle ConnectToRepositoryHandle = FDelegateHandle{};
    FDelegateHandle DisconnectToRepositoryHandle = FDelegateHandle{};

    ARRanger::Repository::IARRepositoryGateway<UObject*, ARRanger::Physics::FMagneticParameterDTO>* m_repositoryConnection = nullptr;

    float Time = 3.0f;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UEditorUtilityButton> TestButton;

    // Slate Panel
    TSharedPtr<SMagneticParamEditPanelWidget> OnScreenPanelWidget;
    TSharedPtr<SWidget> OnScreenPanelWidgetContainer;


};
