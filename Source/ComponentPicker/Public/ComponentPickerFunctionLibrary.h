// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponentPicker.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComponentPickerFunctionLibrary.generated.h"

/**
 * Utility methods for @FActorComponentPicker.
 */
UCLASS(meta=(BlueprintThreadSafe, ScriptName = "ComponentPickerLibrary"), MinimalAPI)
class UComponentPickerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Get the component selected in a @FActorComponentPicker. */
    UFUNCTION(
        BlueprintPure,
        meta=(DisplayName="Get (Component Picker)", ScriptMethod="GetPickedComponent", DeterminesOutputType="ComponentClass"),
        Category="Component Picker")
    static COMPONENTPICKER_API UActorComponent* GetPickedComponent(
        const FActorComponentPicker& Target,
        const TSubclassOf<UActorComponent> ComponentClass);
};
