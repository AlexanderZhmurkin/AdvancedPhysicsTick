// Copyright Alexander. All Rights Reserved.

using UnrealBuildTool;

public class ExtendedAsyncPawn : ModuleRules
{
	public ExtendedAsyncPawn(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
            new string[]
            {
				"Core",
				"PhysicsCore",
				"InputCore",
				"DeveloperSettings" //TODO
            });

		PrivateDependencyModuleNames.AddRange(
            new string[]
            {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Chaos"
            });

		SetupModulePhysicsSupport(Target);
		PrivateDefinitions.Add("CHAOS_INCLUDE_LEVEL_1=1");
	}
}