// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

using UnrealBuildTool;

public class ComponentPickerEditor : ModuleRules
{
    public ComponentPickerEditor(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core", 
                "Kismet",
                "DetailCustomizations",
                "PropertyEditor",
                "SubobjectEditor",
                "EditorFramework",
                "SubobjectDataInterface", 
                "ComponentPicker", 
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}