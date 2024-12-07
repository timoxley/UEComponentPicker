// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

using UnrealBuildTool;

public class ComponentPicker : ModuleRules
{
	public ComponentPicker(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
	}
}
