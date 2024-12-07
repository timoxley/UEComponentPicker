// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Editor module for the component picker plugin, registering details customizations.
 */
class FComponentPickerEditorModule final : public IModuleInterface
{
public:
    // - IModuleInterface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // --
};
