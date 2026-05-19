// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GASTacticalCommand : ModuleRules
{
	public GASTacticalCommand(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
