// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorComponentPicker.generated.h"

/**
 * Allows picking a component via a drop-down menu at editor-time
 * and the retrieving it in C++/BP at runtime.
 *
 * This allows for a more Unity-like dependency injection pattern,
 * where component devs don't need to provide any kind of setter
 * method that has to be manually called to inject the dependency.
 */
USTRUCT(BlueprintType)
struct COMPONENTPICKER_API FActorComponentPicker
{
	GENERATED_BODY()

    /** The currently selected component. */
    UPROPERTY(EditAnywhere, meta=(NoEditInline), Category=ComponentPicker)
    UActorComponent* Component = nullptr;

    /** Get the currently selected component pre-casted. */
    template<typename TComponentClass>
    TComponentClass* Get()
    {
        return Cast<TComponentClass*>(Component);
    }
};
