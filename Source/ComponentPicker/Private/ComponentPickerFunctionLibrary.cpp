// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#include "ComponentPickerFunctionLibrary.h"

UActorComponent* UComponentPickerFunctionLibrary::GetPickedComponent(
    const UActorComponent* Target,
    const FComponentPicker& Picker,
    const TSubclassOf<UActorComponent> ComponentClass)
{
    (void) ComponentClass;
    return Picker.Get(Target);
}
