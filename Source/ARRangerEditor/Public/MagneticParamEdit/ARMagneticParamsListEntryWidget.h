// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditorUtilityWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ARMagneticParamsListEntryWidget.generated.h"

class UTextBlock;
class UEditableTextBox;

UCLASS()
class ARRANGEREDITOR_API UARMagneticParamsListEntryWidget : public UEditorUtilityWidget,
                                                            public IUserObjectListEntry
{
	GENERATED_BODY()

private:
  UPROPERTY(meta = (BindWidgetOptional))
  TObjectPtr<class UTextBlock> TargetNameLabel;

  UPROPERTY(meta = (BindWidgetOptional))
  TObjectPtr<class UEditableTextBox> TargetMassEditTextBox;
	
  UPROPERTY(meta = (BindWidgetOptional))
  TObjectPtr<class UEditableTextBox> TargetMagneticPoleStrengthEditTextBox;
};
