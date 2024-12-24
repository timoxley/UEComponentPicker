﻿// Copyright 2024 Gregor Sönnichsen.
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"

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
	TComponentClass* Get(const UActorComponent* ComponentContext) const
	{
		return Cast<TComponentClass>(Get(ComponentContext));
	}

	/** Get the currently selected component. */
	UActorComponent* Get(const UActorComponent* ComponentContext) const
	{
		if (!Instance.IsExplicitlyNull())
			return Instance.Get();
		if (!Component)
			return nullptr;
		FString ComponentStr = Component->GetFName().ToString();
		ComponentStr.RemoveFromEnd("_GEN_VARIABLE");
		const FName ComponentName = FName(ComponentStr);
		
		const TSet<UActorComponent*>& OwnedComponents = ComponentContext->GetOwner()->GetComponents();
		for (auto* ComponentInstance : OwnedComponents)
		{
			if (ComponentInstance->GetName() == ComponentName)
			{
				const_cast<FComponentPicker*>(this)->Instance = ComponentInstance;
				return ComponentInstance;
			}
		}
		return Component;
	}

	FORCEINLINE void Pick(UActorComponent* PickedComponent)
	{
		Component = PickedComponent;
	}

	FORCEINLINE void SetAllowedClass(const TSubclassOf<UActorComponent>& NewAllowedClass)
	{
		AllowedClass = NewAllowedClass;
	}

	/** Allowed component class. */
	UPROPERTY(EditDefaultsOnly, Category=ComponentPicker)
	TSubclassOf<UActorComponent> AllowedClass = UActorComponent::StaticClass();
	
    /** The currently selected component. */
    UPROPERTY(EditAnywhere, meta=(NoEditInline), Category=ComponentPicker)
    UActorComponent* Component = nullptr;

	TWeakObjectPtr<UActorComponent> Instance = nullptr;
};
