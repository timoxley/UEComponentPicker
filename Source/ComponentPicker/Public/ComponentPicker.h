// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "Modules/ModuleManager.h"

class FComponentPickerModule : public IModuleInterface
{
public:

	// - IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// --
};
