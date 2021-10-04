// Copyright Brace Yourself Games. All Rights Reserved.

using UnrealBuildTool;

public class BYGImguiLogger : ModuleRules
{
	public BYGImguiLogger(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				 "Projects",
				"ImGui",
			}
		);
	}
}
