// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.


#include "ActorComponentPickerTypeCustomization.h"

#include "ActorComponentPicker.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "PublicPropertyEditorButton.h"
#include "SSubobjectBlueprintEditor.h"
#include "SSubobjectEditor.h"
#include "Toolkits/ToolkitManager.h"

#define LOCTEXT_NAMESPACE "FComponentPickerTypeCustomization"

void FActorComponentPickerTypeCustomization::CustomizeHeader(
    TSharedRef<IPropertyHandle> PropertyHandle,
     FDetailWidgetRow& HeaderRow,
     IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    PropHandle = PropertyHandle;
    ComponentPropHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FActorComponentPicker, Component));

    if (Editor = FetchBlueprintEditor(PropertyHandle); !Editor) // editor is unavailable
    {
        HeaderRow
        .NameContent()
        [
             PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MaxDesiredWidth(FDetailWidgetRow::DefaultValueMaxWidth * 2)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("ActorComponentPickerUnavailableDueToNoEditor", "Component picker unavailable outside of blueprints!"))
            .Font(IDetailLayoutBuilder::GetDetailFont())
        ];
    }
    else // editor is available
    {
        HeaderRow
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MaxDesiredWidth(FDetailWidgetRow::DefaultValueMaxWidth * 2)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SAssignNew(ComponentListComboButton, SComboButton)
                .ButtonStyle(FAppStyle::Get(), "PropertyEditor.AssetComboStyle")
                .ForegroundColor(FAppStyle::GetColor("PropertyEditor.AssetName.ColorAndOpacity"))
                .OnGetMenuContent(this, &FActorComponentPickerTypeCustomization::BuildPopupContent)
                .ContentPadding(2.0f)
                .ButtonContent()
                [
                    SNew(STextBlock)
                    .Text(this, &FActorComponentPickerTypeCustomization::HandleGetCurrentComponentName)
                    .Font(IDetailLayoutBuilder::GetDetailFont())
                ]
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SPublicPropertyEditorButton)
                .Text(LOCTEXT("ActorComponentPickerClearButtonText", "Clear"))
                .Image(FAppStyle::GetBrush("Icons.X"))
                .OnClickAction(FSimpleDelegate::CreateLambda([this]
                {
                    SetComponent(nullptr);
                }))
                .IsFocusable(false)
            ]
        ];
    }
}

void FActorComponentPickerTypeCustomization::CustomizeChildren(
    TSharedRef<IPropertyHandle> PropertyHandle,
   IDetailChildrenBuilder& ChildBuilder,
   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
    // no action needed here, but override is required
}

TSharedRef<SWidget> FActorComponentPickerTypeCustomization::BuildPopupContent()
{
    TArray<TSharedRef<IClassViewerFilter>> ClassFilters;
    if (Editor->GetImportedClassViewerFilter().IsValid())
    {
        ClassFilters.Add(Editor->GetImportedClassViewerFilter().ToSharedRef());
    }

    SubobjectEditor = SAssignNew(SubobjectEditor, SSubobjectBlueprintEditor)
        .ObjectContext(this, &FActorComponentPickerTypeCustomization::HandleGetSubobjectEditorObjectContext)
        .PreviewActor(this, &FActorComponentPickerTypeCustomization::HandleGetPreviewActor)
        .AllowEditing(false)
        .HideComponentClassCombo(true)
        .OnSelectionUpdated(this, &FActorComponentPickerTypeCustomization::HandleSelectionUpdated)
        .OnItemDoubleClicked(this, &FActorComponentPickerTypeCustomization::HandleComponentDoubleClicked)
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

UActorComponent* FActorComponentPickerTypeCustomization::ExtractCurrentValueComponent(const TSharedPtr<IPropertyHandle>& PropHandle)
{
    switch (UObject* Value; PropHandle->GetValue(Value))
    {
        case FPropertyAccess::Success:
            return Cast<UActorComponent>(Value);
        default:
            return nullptr;
    }
}

FText FActorComponentPickerTypeCustomization::HandleGetCurrentComponentName() const
{
    const UActorComponent* Component = ExtractCurrentValueComponent(ComponentPropHandle);
    return Component ? FText::FromName(Component->GetFName()) : FText::FromString("None");
}

UObject* FActorComponentPickerTypeCustomization::HandleGetSubobjectEditorObjectContext() const
{
    return Editor->GetSubobjectEditorObjectContext();
}

AActor* FActorComponentPickerTypeCustomization::HandleGetPreviewActor() const
{
    return Editor->GetPreviewActor();
}

void FActorComponentPickerTypeCustomization::HandleSelectionUpdated(const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& SelectedNodes)
{
    UActorComponent* EditableComponent = ExtractComponentFromSubobjectNode(SelectedNodes[0]);
    SetComponent(EditableComponent);

    ComponentListComboButton->SetIsOpen(false);
}

void FActorComponentPickerTypeCustomization::HandleComponentDoubleClicked(TSharedPtr<FSubobjectEditorTreeNode> Node)
{
    UActorComponent* EditableComponent = ExtractComponentFromSubobjectNode(Node);
    SetComponent(EditableComponent);
}

UActorComponent* FActorComponentPickerTypeCustomization::ExtractComponentFromSubobjectNode(
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

void FActorComponentPickerTypeCustomization::SetComponent(UActorComponent* Component) const
{
    AActor* ActorCDO = FetchActorCDOForProperty(PropHandle);
    if (!ActorCDO)
        return;

    // Actual transaction starts here
    const FScopedTransaction Transaction(FText::Format(
        LOCTEXT("SetActorComponentPickerComponentProperty", "Set {0}"),
        ComponentPropHandle->GetPropertyDisplayName()));

    ActorCDO->SetFlags(RF_Transactional);
    ActorCDO->Modify();
    
    ComponentPropHandle->SetValue(Component);
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

            for (UObject* EditedAsset : GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetAllEditedAssets())
            {
                if (const UBlueprint* EditedBlueprintAsset = Cast<UBlueprint>(EditedAsset))
                {
                    if (EditedBlueprintAsset->GeneratedClass == Outer)
                    {
                        if (UObject* CDO = Cast<UClass>(Outer)->ClassDefaultObject)
                        {
                            if (AActor* ActorCDO = Cast<AActor>(CDO))
                            {
                                return ActorCDO;
                            }
                        }
                    }
                }
            }

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

#undef LOCTEXT_NAMESPACE
