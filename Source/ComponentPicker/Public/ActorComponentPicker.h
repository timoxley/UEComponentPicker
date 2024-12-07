// Copyright 2024 Gregor Sönnichsen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorComponentPicker.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct COMPONENTPICKER_API FActorComponentPicker
{
	GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(NoEditInline))
    UActorComponent* Component = nullptr;
};
