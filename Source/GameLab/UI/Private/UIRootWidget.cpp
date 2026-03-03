// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLab/UI/Public/UIRootWidget.h"

#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

// CommonUI stack/container types live here in UE 5.6
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "GameLab/UI/Public/AutoFocusLocalPlayerSubsystem.h"

UAutoFocusLocalPlayerSubsystem* UUIRootWidget::GetAutoFocusSubsystem() const
{
	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		return LP->GetSubsystem<UAutoFocusLocalPlayerSubsystem>();
	}
	return nullptr;
}

void UUIRootWidget::RegisterAsAutoFocusRoot()
{
	if (UAutoFocusLocalPlayerSubsystem* AF = GetAutoFocusSubsystem())
	{
		AF->SetRootActivatable(this);
	}
}

void UUIRootWidget::RequestAutoFocusRestoreIfNeeded()
{
	UAutoFocusLocalPlayerSubsystem* AF = GetAutoFocusSubsystem();
	if (!AF)
	{
		return;
	}

	// Restore focus if gamepad OR "keyboard-like" (mouse not recently active).
	if (AF->GetCurrentInputType() == ECommonInputType::Gamepad || !AF->IsMouseRecentlyActive())
	{
		AF->RestoreBestFocus();
	}
}

void UUIRootWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	RegisterAsAutoFocusRoot();
	RequestAutoFocusRestoreIfNeeded();
}

UCommonActivatableWidget* UUIRootWidget::PushMain(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!MainStack || !WidgetClass)
	{
		return nullptr;
	}

	UCommonActivatableWidget* Pushed = MainStack->AddWidget(WidgetClass);
	RequestAutoFocusRestoreIfNeeded();
	return Pushed;
}

UCommonActivatableWidget* UUIRootWidget::PushModal(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!ModalStack || !WidgetClass)
	{
		return nullptr;
	}

	UCommonActivatableWidget* Pushed = ModalStack->AddWidget(WidgetClass);

	// Modals should almost always restore focus (especially on gamepad).
	RequestAutoFocusRestoreIfNeeded();
	return Pushed;
}

bool UUIRootWidget::PopMain(bool bPreferDeactivate)
{
	if (!MainStack)
	{
		return false;
	}

	if (UCommonActivatableWidget* Active = MainStack->GetActiveWidget())
	{
		if (bPreferDeactivate)
		{
			// CommonUI-style: stack auto-removes the top widget when it deactivates.
			Active->DeactivateWidget();
		}
		else
		{
			MainStack->RemoveWidget(*Active);
		}

		RequestAutoFocusRestoreIfNeeded();
		return true;
	}

	return false;
}

bool UUIRootWidget::PopModal(bool bPreferDeactivate)
{
	if (!ModalStack)
	{
		return false;
	}

	if (UCommonActivatableWidget* Active = ModalStack->GetActiveWidget())
	{
		if (bPreferDeactivate)
		{
			Active->DeactivateWidget();
		}
		else
		{
			ModalStack->RemoveWidget(*Active);
		}

		RequestAutoFocusRestoreIfNeeded();
		return true;
	}

	return false;
}

UCommonActivatableWidget* UUIRootWidget::GetTopmostActivatable() const
{
	// Modal has priority
	if (ModalStack)
	{
		if (UCommonActivatableWidget* TopModal = ModalStack->GetActiveWidget())
		{
			return TopModal;
		}
	}

	return MainStack ? MainStack->GetActiveWidget() : nullptr;
}

UWidget* UUIRootWidget::NativeGetDesiredFocusTarget() const
{
	if (UCommonActivatableWidget* Top = GetTopmostActivatable())
	{
		if (UWidget* Desired = Top->GetDesiredFocusTarget())
		{
			return Desired;
		}
	}

	return Super::NativeGetDesiredFocusTarget();
}
