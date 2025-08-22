// Fill out your copyright notice in the Description page of Project Settings.
// Created by: Alexander Zhmurkin

using UnrealBuildTool;

public class AdvancedPhysicsEditor : ModuleRules
{
    public AdvancedPhysicsEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "AdvancedPhysics"
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