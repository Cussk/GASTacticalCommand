// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASTacticalCommandTarget : TargetRules
{
	public GASTacticalCommandTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("GASTacticalCommand");
		RegisterModulesCreatedByRider();
	}

	void RegisterModulesCreatedByRider()
	{
		ExtraModuleNames.AddRange(new string[] { "GASTacticalCommandDebug", "GASTacticalCommandCore" });
	}
}
