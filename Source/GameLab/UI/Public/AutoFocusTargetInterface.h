// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AutoFocusTargetInterface.generated.h"

class UWidget;

UINTERFACE(BlueprintType)
class GAMELAB_API UAutoFocusTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface used by wrapper widgets to expose the actual focusable internal widget
 * (e.g. CommonButtonBase, CommonSlider, CommonRotator, etc.).
 */
class GAMELAB_API IAutoFocusTargetInterface
{
	GENERATED_BODY()

public:
	/** Return the widget that should actually receive focus (internal control). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="UI|AutoFocus",
		meta=(ToolTip="Return the internal widget that should receive actual focus (e.g. CommonButtonBase)."))
	UWidget* GetAutoFocusTarget() const;
};
