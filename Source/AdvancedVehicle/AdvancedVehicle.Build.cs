// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class AdvancedVehicle : ModuleRules
{
    public AdvancedVehicle(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "PhysicsCore",
                "InputCore",
                "RenderCore",
                "EnhancedInput"
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