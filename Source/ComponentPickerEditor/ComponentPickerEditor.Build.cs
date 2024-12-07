using UnrealBuildTool;

public class ComponentPickerEditor : ModuleRules
{
    public ComponentPickerEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
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
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}