// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CrazyBingo : ModuleRules
{
	public CrazyBingo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG", 
			"Slate", 
			"SlateCore",
			"CommonUI", 
			"CommonInput",
			"Paper2D",
			"RenderCore",
			"RHI",
			"WebBrowser",
			"WebBrowserWidget",
			"ImageWrapper",
			"AppFramework",
			"GameplayTags"
		});

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicSystemLibraries.Add("comdlg32.lib");
		}
		
		PrivateDependencyModuleNames.AddRange(new string[] { });
		
		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory,
			"CrazyBingo" 
		});
	
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}