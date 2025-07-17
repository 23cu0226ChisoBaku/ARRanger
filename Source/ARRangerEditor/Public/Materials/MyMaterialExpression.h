// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Materials/MaterialExpression.h"

#include "MyMaterialExpression.generated.h"

class FMaterialCompiler;

UCLASS(MinimalAPI, collapsecategories, hidecategories=Object)
class UMyMaterialExpression : public UMaterialExpression
{
	GENERATED_BODY()

  UPROPERTY(meta = (RequiredInput = "true"))
  FExpressionInput InputUV;

#if WITH_EDITOR
  virtual int32 Compile(FMaterialCompiler* Compiler, int32 OutputIndex) override;
  virtual void GetCaption(TArray<FString>& OutCaptions) const override;
  virtual bool IsInputConnectionRequired(int32 InputIndex) const override;
#endif
    
};
