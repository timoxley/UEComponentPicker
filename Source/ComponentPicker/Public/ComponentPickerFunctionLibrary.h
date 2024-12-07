// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComponentPickerStruct.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComponentPickerFunctionLibrary.generated.h"

/**
 * Utility methods for @FComponentPicker.
 */
UCLASS(meta=(BlueprintThreadSafe, ScriptName = "ComponentPickerLibrary"), MinimalAPI)
class UComponentPickerFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Get the component selected in a @FComponentPicker. */
    UFUNCTION(
        BlueprintPure,
        meta=(DisplayName="Get (Component Picker)", ScriptMethod="GetPickedComponent", DeterminesOutputType="ComponentClass"),
        Category="Component Picker")
    static COMPONENTPICKER_API UActorComponent* GetPickedComponent(
        const FComponentPicker& Target,
        const TSubclassOf<UActorComponent> ComponentClass);
};
