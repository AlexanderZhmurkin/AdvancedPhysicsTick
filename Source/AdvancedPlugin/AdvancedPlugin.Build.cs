// Copyright Alexander. All Rights Reserved.

using UnrealBuildTool;

public class AdvancedPlugin : ModuleRules
{
    public AdvancedPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "PhysicsCore",
                "InputCore"
            });

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Engine",
                "Slate",
                "SlateCore",
                "Chaos"
            });

        SetupModulePhysicsSupport(Target);
        PrivateDefinitions.Add("CHAOS_INCLUDE_LEVEL_1=1");
    }
}