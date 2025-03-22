// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class FightingGame : ModuleRules
{
	public FightingGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PublicDependencyModuleNames.AddRange(new string[] {"UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemNull", "OnlineSubsystemEOS" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        // Set EOS SDK path
        string EOS_SDK_PATH = Path.Combine(ModuleDirectory, "../../Plugins/EOS_SDK/SDK");

        // Include Headers
        PublicIncludePaths.Add(Path.Combine(EOS_SDK_PATH, "Include"));

        // Add Library
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(EOS_SDK_PATH, "Lib", "EOSSDK-Win64-Shipping.lib"));
            RuntimeDependencies.Add(Path.Combine(EOS_SDK_PATH, "lib", "EOSSDK-Win64-Shipping.dll"));
        }
    }
}
