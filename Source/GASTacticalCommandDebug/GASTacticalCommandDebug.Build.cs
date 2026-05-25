using UnrealBuildTool;

public class GASTacticalCommandDebug : ModuleRules
{
	public GASTacticalCommandDebug(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"GASTacticalCommand"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore"
		});
	}
}
