// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ClassViewerFilter.h"

class FComponentClassFilter final : public IClassViewerFilter
{
public:
	/** All children of these classes will be included. */
	TSet<const UClass*> AllowedParentClasses;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return InFilterFuncs->IfInChildOfClassesSet(AllowedParentClasses, InClass) != EFilterReturn::Failed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InFilterFuncs->IfInChildOfClassesSet(AllowedParentClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};
