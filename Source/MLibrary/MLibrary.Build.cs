// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MLibrary: ModuleRules
{
  public MLibrary(ReadOnlyTargetRules Target) : base(Target)
  {

    PublicDependencyModuleNames.AddRange(new string[] {"Core" , "CoreUObject" , "Engine"});

  }

}