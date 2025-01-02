// Copyright Alexander. All Rights Reserved.

using UnrealBuildTool;

public class AdvancedPluginEditor : ModuleRules
{
    public AdvancedPluginEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "Engine",
                "EditorFramework",
                "AdvancedPlugin"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd",
                "PropertyEditor",
                "Engine",
                "EngineSettings"
            });
    }
}