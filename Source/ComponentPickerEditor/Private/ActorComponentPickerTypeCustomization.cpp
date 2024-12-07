// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.


#include "ActorComponentPickerTypeCustomization.h"

#include "ComponentPickerSCSEditorUICustomization.h"
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
    ComponentPropHandle = PropertyHandle->GetChildHandle("Component");
    AllowedClassPropHandle = PropertyHandle->GetChildHandle("AllowedClass");

    BlueprintToolkit = FetchBlueprintEditor(PropertyHandle);

    HeaderRow
    .NameContent()
    [
        PropertyHandle->CreatePropertyNameWidget()
    ]
    .ValueContent()
    .MaxDesiredWidth(FDetailWidgetRow::DefaultValueMaxWidth * 2)
    [
        SNew(SVerticalBox)
        // component picker
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            .IsEnabled_Lambda([this](){ return BlueprintToolkit != nullptr; })
            .ToolTipText(LOCTEXT("ComponentPickerToolTipText",
                "Pick the component to be accessed later on. Only available in the blueprint actor editor."))
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
                    TrySetComponent(nullptr);
                }))
                .IsFocusable(false)
            ]
        ]
        // spacer
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SSpacer)
            .Size(FVector2D(0.f, 5.f))
        ]
        // allowed class selection
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .IsEnabled_Lambda([this](){ return BlueprintToolkit == nullptr; })
            .ToolTipText(LOCTEXT("AllowedComponentToolTipText",
                "Choose the component class that is allowed to be picked. Only available in the blueprint component editor."))
            [
                AllowedClassPropHandle->CreatePropertyValueWidget()
            ]
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

TSharedRef<SWidget> FActorComponentPickerTypeCustomization::BuildPopupContent()
{
    RebuildClassFilters();
    
    SubobjectEditor = SAssignNew(SubobjectEditor, SSubobjectBlueprintEditor)
        .ObjectContext(this, &FActorComponentPickerTypeCustomization::HandleGetSubobjectEditorObjectContext)
        .PreviewActor(this, &FActorComponentPickerTypeCustomization::HandleGetPreviewActor)
        .AllowEditing(false)
        .HideComponentClassCombo(false)
        .OnSelectionUpdated(this, &FActorComponentPickerTypeCustomization::HandleSelectionUpdated)
        .OnItemDoubleClicked(this, &FActorComponentPickerTypeCustomization::HandleComponentDoubleClicked);

    SubobjectEditor->SetUICustomization(FComponentPickerSCSEditorUICustomization::GetInstance());

    constexpr float MinPopupWidth = 250.0f;
    constexpr float MinPopupHeight = 200.0f;

    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("Brushes.Secondary"))
        .Padding(2.f, 6.f)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::Get().GetBrush("Brushes.Recessed"))
            .Padding(4.0f)
            [
                SNew(SBox)
                .MinDesiredWidth(MinPopupWidth)
                .MinDesiredHeight(MinPopupHeight)
                [
                    SubobjectEditor.ToSharedRef()
                ]
            ]
        ];
}

void FActorComponentPickerTypeCustomization::RebuildClassFilters() const
{
    FComponentPickerSCSEditorUICustomization::GetInstance()->SetComponentTypeFilter(ExtractAllowedComponentClass(AllowedClassPropHandle));
}

FText FActorComponentPickerTypeCustomization::HandleGetCurrentComponentName() const
{
    const UActorComponent* Component = ExtractCurrentlyPickedComponent(ComponentPropHandle);
    if (!Component)
    {
        return FText::FromString("None");
    }

    return FText::FromString(Component->GetName());

    //todo: check FSubobjectData::GetDisplayString to get the actually displayed string
    
    // const UClass* ComponentClass = Component->GetClass();
    //
    // // try get display name
    // if (ComponentClass->HasMetaData(TEXT("DisplayName")))
    // {
    //     return FText::FromString(ComponentClass->GetMetaData(TEXT("DisplayName")));
    // }
    //
    // // get default display name text, adjusted for blueprint classes
    // FString DisplayName = ComponentClass->GetDisplayNameText().ToString();
    // if (!ComponentClass->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
    // {
    //     DisplayName.RemoveFromEnd(TEXT("Component"), ESearchCase::IgnoreCase);
    // }
    // return FText::FromString(DisplayName);
}

UObject* FActorComponentPickerTypeCustomization::HandleGetSubobjectEditorObjectContext() const
{
    return BlueprintToolkit->GetSubobjectEditorObjectContext();
}

AActor* FActorComponentPickerTypeCustomization::HandleGetPreviewActor() const
{
    return BlueprintToolkit->GetPreviewActor();
}

void FActorComponentPickerTypeCustomization::HandleSelectionUpdated(const TArray<TSharedPtr<FSubobjectEditorTreeNode>>& SelectedNodes)
{
    UActorComponent* EditableComponent = ExtractComponentFromSubobjectNode(SelectedNodes[0]);
    TrySetComponent(EditableComponent);

    ComponentListComboButton->SetIsOpen(false);
}

void FActorComponentPickerTypeCustomization::HandleComponentDoubleClicked(TSharedPtr<FSubobjectEditorTreeNode> Node)
{
    UActorComponent* EditableComponent = ExtractComponentFromSubobjectNode(Node);
    TrySetComponent(EditableComponent);

    ComponentListComboButton->SetIsOpen(false);
}

UClass* FActorComponentPickerTypeCustomization::ExtractAllowedComponentClass(
    const TSharedPtr<IPropertyHandle>& PropHandle)
{
    switch (UObject* Value; PropHandle->GetValue(Value))
    {
    case FPropertyAccess::Success:
        return Cast<UClass>(Value);
    default:
        return nullptr;
    }
}

UActorComponent* FActorComponentPickerTypeCustomization::ExtractCurrentlyPickedComponent(const TSharedPtr<IPropertyHandle>& PropHandle)
{
    switch (UObject* Value; PropHandle->GetValue(Value))
    {
    case FPropertyAccess::Success:
        return Cast<UActorComponent>(Value);
    default:
        return nullptr;
    }
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

void FActorComponentPickerTypeCustomization::TrySetComponent(UActorComponent* Component) const
{
    const UClass* AllowedClass = ExtractAllowedComponentClass(AllowedClassPropHandle);
    if (Component != nullptr && AllowedClass != nullptr)
    {
        if (!Component->IsA(AllowedClass)) return;
    }
    
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
