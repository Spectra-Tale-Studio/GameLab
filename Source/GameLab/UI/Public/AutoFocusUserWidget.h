// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "AutoFocusUserWidget.generated.h"

class UAutoFocusLocalPlayerSubsystem;

/**
 * Base wrapper widget that notifies AutoFocus subsystem
 * when hovered or added to focus path.
 */
UCLASS()
class GAMELAB_API UAutoFocusUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()

protected:

	/** Called when mouse enters this widget */
	virtual void NativeOnMouseEnter(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	/** Called when widget receives keyboard/gamepad focus */
	virtual void NativeOnAddedToFocusPath(
		const FFocusEvent& InFocusEvent
	) override;

	/** Called when mouse moves over widget */
	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

private:

	UAutoFocusLocalPlayerSubsystem* GetAutoFocusSubsystem() const;
};
