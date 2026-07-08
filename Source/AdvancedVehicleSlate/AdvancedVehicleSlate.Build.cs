// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class AdvancedVehicleSlate : ModuleRules
{
    public AdvancedVehicleSlate(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));

        PublicDependencyModuleNames.AddRange(
              new string[]
              {
                        "Core",
                        "CoreUObject",
                        "Slate",
                        "SlateCore",
                        "Engine",
                        
              });

        PrivateDependencyModuleNames.AddRange(
              new string[]
              {
                        "RHI",
                        "RenderCore",
                        "Projects",
                        "AdvancedVehicle"
              });
    }
}
