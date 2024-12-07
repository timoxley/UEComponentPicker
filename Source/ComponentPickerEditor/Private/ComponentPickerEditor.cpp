// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#include "ComponentPickerEditor.h"

#include "ActorComponentPickerTypeCustomization.h"

void FComponentPickerEditorModule::StartupModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
    PropertyModule.RegisterCustomPropertyTypeLayout
    (
        TEXT("ActorComponentPicker"),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FActorComponentPickerTypeCustomization::MakeInstance)
    );
}

void FComponentPickerEditorModule::ShutdownModule()
{
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("ActorComponentPicker"));
    }
}
    
IMPLEMENT_MODULE(FComponentPickerEditorModule, ComponentPickerEditor)