// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#include "ComponentPickerFunctionLibrary.h"

UActorComponent* UComponentPickerFunctionLibrary::GetPickedComponent(
    const FComponentPicker& Target,
    const UActorComponent* ComponentContext,
    const TSubclassOf<UActorComponent> ComponentClass)
{
    (void) ComponentClass;
    return Target.Get(ComponentContext);
}
