// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

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
