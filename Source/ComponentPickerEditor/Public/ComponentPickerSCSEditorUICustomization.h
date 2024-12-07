// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ISCSEditorUICustomization.h"

class FComponentPickerSCSEditorUICustomization final : public ISCSEditorUICustomization
{
public:
	static TSharedPtr<FComponentPickerSCSEditorUICustomization> GetInstance();

	FComponentPickerSCSEditorUICustomization() {}
	
	void SetComponentTypeFilter(const TSubclassOf<UActorComponent>& InComponentTypeFilter);

	// - ISCSEditorUICustomization
	virtual ~FComponentPickerSCSEditorUICustomization() override {}
	virtual bool HideComponentsTree(TArrayView<UObject*> Context) const override;
	virtual bool HideComponentsFilterBox(TArrayView<UObject*> Context) const override;
	virtual bool HideAddComponentButton(TArrayView<UObject*> Context) const override;
	virtual bool HideBlueprintButtons(TArrayView<UObject*> Context) const override;
	virtual EChildActorComponentTreeViewVisualizationMode GetChildActorVisualizationMode() const override;
	virtual TSubclassOf<UActorComponent> GetComponentTypeFilter(TArrayView<UObject*> Context) const override;
	// --

private:
	TSubclassOf<UActorComponent> ComponentTypeFilter = nullptr;
	static TSharedPtr<FComponentPickerSCSEditorUICustomization> Instance;
};
