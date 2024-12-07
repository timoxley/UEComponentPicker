// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ComponentPickerStruct.generated.h"

/**
 * Allows picking a component via a drop-down menu at editor-time
 * and the retrieving it in C++/BP at runtime.
 *
 * This allows for a more Unity-like dependency injection pattern,
 * where component devs don't need to provide any kind of setter
 * method that has to be manually called to inject the dependency.
 */
USTRUCT(BlueprintType)
struct COMPONENTPICKER_API FComponentPicker
{
	GENERATED_BODY()

public:
	/** Get the currently selected component pre-casted. */
	template<typename TComponentClass>
	TComponentClass* Get() const
	{
		return Cast<TComponentClass*>(Component);
	}

	/** Get the currently selected component. */
	FORCEINLINE UActorComponent* Get() const
	{
		return Component;
	}

	FORCEINLINE void SetAllowedClass(const TSubclassOf<UActorComponent>& NewAllowedClass)
	{
		AllowedClass = NewAllowedClass;
	}

protected:
	/** Allowed component class. */
	UPROPERTY(EditDefaultsOnly, Category=ComponentPicker)
	TSubclassOf<UActorComponent> AllowedClass = UActorComponent::StaticClass();
	
    /** The currently selected component. */
    UPROPERTY(EditAnywhere, meta=(NoEditInline), Category=ComponentPicker)
    UActorComponent* Component = nullptr;
};
