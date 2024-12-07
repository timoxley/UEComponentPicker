// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.


#include "ActorComponentPickerTypeCustomization.h"

#include "ActorComponentPicker.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "SSubobjectBlueprintEditor.h"
#include "SSubobjectEditor.h"
#include "Toolkits/ToolkitManager.h"

void FActorComponentPickerTypeCustomization::CustomizeHeader(
    TSharedRef<IPropertyHandle> PropertyHandle,
     FDetailWidgetRow& HeaderRow,
     IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    PropHandle = PropertyHandle;
    ComponentPropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FActorComponentPicker, Component));

    if (Editor = FetchBlueprintEditor(PropertyHandle); !Editor)
        return;

    HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MaxDesiredWidth(FDetailWidgetRow::DefaultValueMaxWidth * 2)
        [
            SAssignNew(ComponentListComboButton, SComboButton)
            .ButtonStyle(FAppStyle::Get(), "PropertyEditor.AssetComboStyle")
            .ForegroundColor(FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
            .OnGetMenuContent(this, &FActorComponentPickerTypeCustomization::GetPopupContent)
            .ContentPadding(2.0f)
            .ButtonContent()
            [
                SNew(STextBlock)
                .Text(this, &FActorComponentPickerTypeCustomization::GetCurrentValueComponentName)
                .Font(IDetailLayoutBuilder::GetDetailFont())
            ]
        ];
}

void FActorComponentPickerTypeCustomization::CustomizeChildren(
    TSharedRef<IPropertyHandle> PropertyHandle,
   IDetailChildrenBuilder& ChildBuilder,
   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    // no action needed here, but override is required
}

TSharedRef<SWidget> FActorComponentPickerTypeCustomization::GetPopupContent()
{
    TArray<TSharedRef<IClassViewerFilter>> ClassFilters;
    if (Editor->GetImportedClassViewerFilter().IsValid())
    {
        ClassFilters.Add(Editor->GetImportedClassViewerFilter().ToSharedRef());
    }

    SubobjectEditor = SAssignNew(SubobjectEditor, SSubobjectBlueprintEditor)
        .ObjectContext(this, &FActorComponentPickerTypeCustomization::GetSubobjectEditorObjectContext)
        .PreviewActor(this, &FActorComponentPickerTypeCustomization::GetPreviewActor)
        .AllowEditing(false)
        .HideComponentClassCombo(true)
        .OnSelectionUpdated(this, &FActorComponentPickerTypeCustomization::OnSelectionUpdated)
        .OnItemDoubleClicked(this, &FActorComponentPickerTypeCustomization::OnComponentDoubleClicked)
        .SubobjectClassListFilters(ClassFilters);

    constexpr float MinPopupWidth = 250.0f;
    constexpr float MinPopupHeight = 200.0f;

    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
        .Padding(4.0f)
        [
            SNew(SBox)
            .MinDesiredWidth(MinPopupWidth)
            .MinDesiredHeight(MinPopupHeight)
            [
                SubobjectEditor.ToSharedRef()
            ]
        ];
}

UActorComponent* FActorComponentPickerTypeCustomization::GetCurrentValueComponent() const
{
    switch (UObject* Value; ComponentPropHandle->GetValue(Value))
    {
        case FPropertyAccess::Success:
            return Cast<UActorComponent>(Value);
        default:
            return nullptr;
    }
}

FText FActorComponentPickerTypeCustomization::GetCurrentValueComponentName() const
{
    const UActorComponent* Component = GetCurrentValueComponent();
    return Component ? FText::FromName(Component->GetFName()) : FText::FromString("None");
}

UObject* FActorComponentPickerTypeCustomization::GetSubobjectEditorObjectContext() const
{
    return Editor->GetSubobjectEditorObjectContext();
}

AActor* FActorComponentPickerTypeCustomization::GetPreviewActor() const
{
    return Editor->GetPreviewActor();
}

void FActorComponentPickerTypeCustomization::OnSelectionUpdated(const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& SelectedNodes)
{
    UActorComponent* EditableComponent = GetComponentFromSubobjectNode(SelectedNodes[0]);
    AActor* ActorCDO = FetchActorCDOForProperty(PropHandle);
    SetComponent(ActorCDO, EditableComponent);

    ComponentListComboButton->SetIsOpen(false);
}

void FActorComponentPickerTypeCustomization::OnComponentDoubleClicked(TSharedPtr<FSubobjectEditorTreeNode> Node)
{
    UActorComponent* EditableComponent = GetComponentFromSubobjectNode(Node);
    AActor* ActorCDO = FetchActorCDOForProperty(PropHandle);
    SetComponent(ActorCDO, EditableComponent);
}

UActorComponent* FActorComponentPickerTypeCustomization::GetComponentFromSubobjectNode(
    const FSubobjectEditorTreeNodePtrType& SubobjectNodePtr)
{
    if (!SubobjectNodePtr)
        return nullptr;
    
    const FSubobjectData* NodeData = SubobjectNodePtr->GetDataSource();
    if (!NodeData)
        return nullptr;
    
    const UActorComponent* TmpComponent = NodeData->GetObject<UActorComponent>();
    if (!TmpComponent)
        return nullptr;
    
   return const_cast<UActorComponent*>(TmpComponent);
}

void FActorComponentPickerTypeCustomization::SetComponent(AActor* OwnerActorCDO, UActorComponent* Component) const
{
    if (!ensure(OwnerActorCDO))
        return;

    // Actual transaction starts here
    const FScopedTransaction Transaction(FText::Format(
        NSLOCTEXT("ActorComponentPicker", "SetActorComponentPickerComponentProperty", "Set {0}"),
        ComponentPropHandle->GetPropertyDisplayName()));

    OwnerActorCDO->SetFlags(RF_Transactional);
    OwnerActorCDO->Modify();
    
    switch (ComponentPropHandle->SetValue(Component))
    {
        case FPropertyAccess::Fail:
            UE_LOG(LogTemp, Error, TEXT("Setting component property failed!"))
        default:
            {}
    }
}

AActor* FActorComponentPickerTypeCustomization::FetchActorCDOForProperty(
    const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
    TArray<UObject*> ObjectArray;
    PropertyHandle->GetOuterObjects(ObjectArray);
    for (UObject* Object : ObjectArray)
    {
        UObject* Outer;
        UObject* NextOuter = Object;

        do
        {
            Outer = NextOuter;

            if (!IsValid(Outer))
                break;

            if (const auto Actor = Cast<AActor>(Outer))
                return Actor;

            NextOuter = Outer->GetOuter();
        }
        while (Outer != NextOuter);
    }
    return nullptr;
}

FBlueprintEditor* FActorComponentPickerTypeCustomization::FetchBlueprintEditor(
    const TSharedPtr<IPropertyHandle>& PropertyHandle)
{
    const AActor* EditedActor = FetchActorCDOForProperty(PropertyHandle);
    if (!IsValid(EditedActor))
        return nullptr;
    const UClass* Class = EditedActor->GetClass();
    if (!IsValid(Class))
        return nullptr;
    const UObject* Blueprint = Class->ClassGeneratedBy;
    if (!IsValid(Blueprint))
        return nullptr;
    const TSharedPtr<IToolkit> Toolkit = FToolkitManager::Get().FindEditorForAsset(Blueprint);
    if (!Toolkit.IsValid())
        return nullptr;
    return StaticCast<FBlueprintEditor*>(Toolkit.Get());
}
