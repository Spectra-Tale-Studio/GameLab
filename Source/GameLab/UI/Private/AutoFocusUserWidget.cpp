// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLab/UI/Public/AutoFocusUserWidget.h"
#include "GameLab/UI/Public/AutoFocusLocalPlayerSubsystem.h"
#include "Engine/LocalPlayer.h"

UAutoFocusLocalPlayerSubsystem* UAutoFocusUserWidget::GetAutoFocusSubsystem() const
{
	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		return LP->GetSubsystem<UAutoFocusLocalPlayerSubsystem>();
	}
	return nullptr;
}

void UAutoFocusUserWidget::NativeOnMouseEnter(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (UAutoFocusLocalPlayerSubsystem* AF = GetAutoFocusSubsystem())
	{
		AF->NotifyMouseActivity();
		AF->NotifyInteractable(this);
	}
}

FReply UAutoFocusUserWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (UAutoFocusLocalPlayerSubsystem* AF = GetAutoFocusSubsystem())
	{
		AF->NotifyMouseActivity();
	}

	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UAutoFocusUserWidget::NativeOnAddedToFocusPath(
	const FFocusEvent& InFocusEvent
)
{
	Super::NativeOnAddedToFocusPath(InFocusEvent);

	if (UAutoFocusLocalPlayerSubsystem* AF = GetAutoFocusSubsystem())
	{
		AF->NotifyInteractable(this);
	}
}