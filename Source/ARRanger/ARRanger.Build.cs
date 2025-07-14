// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ARRanger : ModuleRules
{
	public ARRanger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"GameplayTags",
		});

		PublicIncludePaths.AddRange(new string[] {
			"ARRanger",
			"ARRanger/Variant_Platforming",
			"ARRanger/Variant_Combat",
			"ARRanger/Variant_Combat/AI",
			"ARRanger/Variant_SideScrolling",
			"ARRanger/Variant_SideScrolling/Gameplay",
			"ARRanger/Variant_SideScrolling/AI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
