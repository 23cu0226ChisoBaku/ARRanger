// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ARRangerEditor : ModuleRules
{
  public ARRangerEditor(ReadOnlyTargetRules Target) : base(Target)
	{

		PrivateDependencyModuleNames.AddRange(
			new string[]
      {
        "ARRanger",
        "Core",
        "CoreUObject",
        "Engine",
        "Slate",
        "SlateCore",
        "UnrealEd",
        "UMG",
        "ScriptableEditorWidgets",
        "PropertyEditor",
        "Persona",
        "AnimationEditor",
			}
		);
	}
}