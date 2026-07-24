// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GASTacticalCommandEditorTarget : TargetRules
{
	public GASTacticalCommandEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
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
