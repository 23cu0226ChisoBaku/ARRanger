// Fill out your copyright notice in the Description page of Project Settings.


#include "MagneticParamEdit/SMagneticParamEditPanelWidget.h"

#include "Repositories/IARRepositoryInterface.h"
#include "Physics/DataTransfer/MagneticParameterDTO.h"

#define LOCTEXT_NAMESPACE "MagneticParamEditPanel"

using ARRanger::Physics::FMagneticParameterDTO;

void SMagneticParamEditPanelWidget::Construct(const FArguments& InArgs)
{
  bCanSupportFocus = true;

  RepositoryConnection = InArgs._RepositoryConnection;

  const FMargin ContentPadding = FMargin{500.0f, 300.0f};
  const FVector2D BackgroundSize = FVector2D{500.0, 500.0};

  const FText TitleText = LOCTEXT("GameTitle", "My Super Great Game");

  const FColor PanelBackgroundColor = FColor{/**R */ 0u,/**G */ 0u,/**B */ 0u,/**A */ 128u };

  TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

  if (RepositoryConnection != nullptr)
  {
    TArray<FMagneticParameterDTO> repositoryDatas{};
    const int32 dataNum = RepositoryConnection->FindAll(repositoryDatas);
    for (int32 idx = 0; idx < dataNum; ++idx)
    {
      float test = repositoryDatas[idx].MagneticCharge;
      VerticalBox->AddSlot()
      .AutoHeight()
      .Padding(8)
      [
        SNew(STextBlock)
        .Text(FText::AsNumber(test))
      ];
    }
  }

  ChildSlot
    [
      SNew(SOverlay)
      + SOverlay::Slot()
      .HAlign(HAlign_Fill)
      .VAlign(VAlign_Fill)
      [
        SNew(SImage)
        .ColorAndOpacity(PanelBackgroundColor)
        .DesiredSizeOverride(BackgroundSize)
      ]
      + SOverlay::Slot()
      .HAlign(HAlign_Fill)
      .VAlign(VAlign_Fill)
      .Padding(ContentPadding)
      [
        VerticalBox.ToSharedRef()
      ]
    ];
}


void SMagneticParamEditPanelWidget::SetRepository(RepositoryTypePtr InRepositoryConnection)
{
  RepositoryConnection = InRepositoryConnection;
}

void SMagneticParamEditPanelWidget::ResetRepository()
{
  RepositoryConnection = nullptr;
}

#undef LOCTEXT_NAMESPACE