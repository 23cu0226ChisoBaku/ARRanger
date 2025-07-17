// Fill out your copyright notice in the Description page of Project Settings.


#include "Materials/MyMaterialExpression.h"
#include "MaterialCompiler.h"

#define LOCTEXT_NAMESPACE "MyMaterialExpression"

#if WITH_EDITOR

UMyMaterialExpression::UMyMaterialExpression(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  #if WITH_EDITORONLY_DATA
    bShowOutputNameOnPin = true;

    Outputs.Reset();
    Outputs.Add({TEXT("SlopeBetweenUV00"), 1, 1, 0, 0, 0}); // float
    Outputs.Add({TEXT("SlopeBetweenUV01"), 1, 1, 0, 0, 0}); // float
    Outputs.Add({TEXT("SlopeBetweenUV10"), 1, 1, 0, 0, 0}); // float
    Outputs.Add({TEXT("SlopeBetweenUV11"), 1, 1, 0, 0, 0}); // float
  #endif
}

int32 UMyMaterialExpression::Compile(FMaterialCompiler* Compiler, int32 OutputIndex)
{
  int32 result = INDEX_NONE;

  if (InputUV.GetTracedInput().Expression == nullptr)
  {
    result = Compiler->Errorf(TEXT("Custom Error from MyMaterialExpression"));
  }
  else
  {
    const int32 inputVal = InputUV.Compile(Compiler);
  
    // Ensure the input is a Vector2(UV)
    const int32 inputUV = Compiler->ForceCast(inputVal, MCT_Float2);
  
    // Calculate slope 
    // UV (0,0) 
    #error Start here
    //Compiler->Sub
    result = inputUV;
  }

  return result;
}

void UMyMaterialExpression::GetCaption(TArray<FString>& OutCaptions) const
{
  OutCaptions.Add(TEXT("MyMaterialExpression"));
}

bool UMyMaterialExpression::IsInputConnectionRequired(int32 InputIndex) const
{
  return true;
}

#endif

#undef LOCTEXT_NAMESPACE
