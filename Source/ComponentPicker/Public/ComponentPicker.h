// Copyright Epic Games, Inc. All Rights Reserved.

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
