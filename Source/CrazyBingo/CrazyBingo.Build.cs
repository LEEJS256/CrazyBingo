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
			// GAS
			"DesktopPlatform",
			"WebBrowser",
			"WebBrowserWidget",
			"ImageWrapper",
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
		
		PublicIncludePaths.AddRange(new string[]
		{
			// 현재 모듈의 Source 폴더 경로를 포함 경로에 추가합니다.
			// 이렇게 하면 "UI/CB_BingoBoard.h" 처럼 내부 폴더 경로로 접근이 가능해집니다.
			ModuleDirectory,
			"CrazyBingo" 
		});
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}