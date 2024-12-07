// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#include "ComponentPickerSCSEditorUICustomization.h"

TSharedPtr<FComponentPickerSCSEditorUICustomization> FComponentPickerSCSEditorUICustomization::Instance;

TSharedPtr<FComponentPickerSCSEditorUICustomization> FComponentPickerSCSEditorUICustomization::GetInstance()
{
	if (!Instance)
	{
		Instance = MakeShareable(new FComponentPickerSCSEditorUICustomization());
	}
	return Instance;
}

void FComponentPickerSCSEditorUICustomization::SetComponentTypeFilter(const TSubclassOf<UActorComponent>& InComponentTypeFilter)
{
	ComponentTypeFilter = InComponentTypeFilter;
}

bool FComponentPickerSCSEditorUICustomization::HideComponentsTree(TArrayView<UObject*> Context) const
{
	return false;
}

bool FComponentPickerSCSEditorUICustomization::HideComponentsFilterBox(TArrayView<UObject*> Context) const
{
	return false;
}

bool FComponentPickerSCSEditorUICustomization::HideAddComponentButton(TArrayView<UObject*> Context) const
{
	return true;
}

bool FComponentPickerSCSEditorUICustomization::HideBlueprintButtons(TArrayView<UObject*> Context) const
{
	return true;
}

EChildActorComponentTreeViewVisualizationMode FComponentPickerSCSEditorUICustomization::GetChildActorVisualizationMode() const
{
	return EChildActorComponentTreeViewVisualizationMode::ComponentOnly;
}

TSubclassOf<UActorComponent> FComponentPickerSCSEditorUICustomization::GetComponentTypeFilter(TArrayView<UObject*> Context) const
{
	return ComponentTypeFilter;
}
