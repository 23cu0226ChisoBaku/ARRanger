// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EditorUtilityWidget.h"

#include "ARMagneticParamEditPanel.generated.h"

UCLASS()
class ARRANGEREDITOR_API UARMagneticParamEditPanel : public UEditorUtilityWidget
{
	GENERATED_BODY()
	

  private:
    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UEditorUtilityWidget> TestWidget;

};
