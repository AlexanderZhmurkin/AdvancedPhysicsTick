// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class AdvancedVehicleEditor : ModuleRules
{
    public AdvancedVehicleEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "Projects",
                "EditorStyle",
                "AdvancedVehicleSlate",
                "AdvancedVehicle"
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