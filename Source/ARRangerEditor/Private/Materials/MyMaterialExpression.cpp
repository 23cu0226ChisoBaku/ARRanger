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
    switch (OutputIndex)
    { 
      // UV (0,0) 
      case 0:
      {
        const int32 vert00 = Compiler->Constant2(0.0f, 0.0f);
        result = Compiler->Min(inputUV, vert00);
      }
      break;
      // UV (0,1)
      case 1:
      {
        const int32 vert01 = Compiler->Constant2(0.0f, 1.0f);
        result = Compiler->Min(inputUV, vert01);
      }
      break;
      // UV (1,0)
      case 2:
      {
        const int32 vert10 = Compiler->Constant2(1.0f, 0.0f);
        result = Compiler->Min(inputUV, vert10);
      }
      break;
      // UV (1,1)
      case 3:
      {
        const int32 vert11 = Compiler->Constant2(1.0f, 1.0f);
        result = Compiler->Min(inputUV, vert11);
      }
      break;
    }

    // Get U of UV vector(Mask(R))
    const int32 resultU = Compiler->ComponentMask(result, true, false, false, false);

    // Get V of UV vector(Mask(G))
    const int32 resultV = Compiler->ComponentMask(result, false, true, false, false);

    // Calculate slope (V divides U)
    result = Compiler->Div(resultU, resultV);
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
