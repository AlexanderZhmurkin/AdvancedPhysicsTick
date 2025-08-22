// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

using UnrealBuildTool;

public class AdvancedPhysics : ModuleRules
{
    public AdvancedPhysics(ReadOnlyTargetRules Target) : base(Target)
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