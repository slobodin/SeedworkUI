// Copyright (c) Mistfly Games. All Rights Reserved.

using UnrealBuildTool;

public class SeedworkUI : ModuleRules
{
    public SeedworkUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core"
        ]);

        PrivateDependencyModuleNames.AddRange(
        [
            "CoreUObject",
            "Engine",
            "SlateCore",
            "Slate",
            "UMG",
            "CommonUI",
            "CommonInput",
            "GameplayTags"
        ]);
    }
}
