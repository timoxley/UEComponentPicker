// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"
#include "SSubobjectEditor.h"


/**
 * 
 */
class FActorComponentPickerTypeCustomization : public IPropertyTypeCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IPropertyTypeCustomization> MakeInstance()
    {
        return MakeShareable(new FActorComponentPickerTypeCustomization());
    }
    
    FActorComponentPickerTypeCustomization()
    {}

    // - IPropertyTypeCustomization
    virtual void CustomizeHeader(
        TSharedRef<IPropertyHandle> PropertyHandle,
        FDetailWidgetRow& HeaderRow,
        IPropertyTypeCustomizationUtils& CustomizationUtils
    ) override;
    virtual void CustomizeChildren(
        TSharedRef<IPropertyHandle> PropertyHandle,
        IDetailChildrenBuilder& ChildBuilder,
        IPropertyTypeCustomizationUtils& CustomizationUtils
    ) override;
    // --
    
private:
    void SetComponent(AActor* OwnerActorCDO, UActorComponent* Component) const;

    TSharedRef<SWidget> GetPopupContent();

    UActorComponent* GetCurrentValueComponent() const;
    FText GetCurrentValueComponentName() const;

    // ---- Subobject Editor Delegates
    UObject* GetSubobjectEditorObjectContext() const;
    AActor* GetPreviewActor() const;
    void OnSelectionUpdated(const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& SelectedNodes);
    void OnComponentDoubleClicked(TSharedPtr<FSubobjectEditorTreeNode> Node);
    // ----

    static UActorComponent* GetComponentFromSubobjectNode(const FSubobjectEditorTreeNodePtrType& SubobjectNodePtr);

    static AActor* FetchActorCDOForProperty(const TSharedPtr<IPropertyHandle>& PropertyHandle);
    static FBlueprintEditor* FetchBlueprintEditor(const TSharedPtr<IPropertyHandle>& PropertyHandle);

private:
    FBlueprintEditor* Editor;
    TSharedPtr<SComboButton> ComponentListComboButton;
    TSharedPtr<SListView<TWeakObjectPtr<UActorComponent>>> ComponentListView;
    TSharedPtr<SSearchBox> SearchBox;
    TSharedPtr<SSubobjectEditor> SubobjectEditor;

    TSharedPtr<IPropertyHandle> PropHandle;
    TSharedPtr<IPropertyHandle> ComponentPropHandle;
    
    TArray<TWeakObjectPtr<UActorComponent>> ReferenceableComponents;
};
