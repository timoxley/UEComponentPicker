// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#include "ComponentPickerFunctionLibrary.h"

UActorComponent* UComponentPickerFunctionLibrary::GetPickedComponent(
    const FComponentPicker& Target,
    const TSubclassOf<UActorComponent> ComponentClass)
{
    (void) ComponentClass;
    return Target.Get();
}
